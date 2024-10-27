/*
 DocumentPageAnnotations - helper to draw indexed geometries over an scaled document image

 For drawing the wxDC::draw*() functions are used

 Data loaded from XML file

 <document>
   <documenttype> uflow </documenttype>
   <pdp11type> pdp1134 </pdp11type>
   <Annotations>
       <Annotation> ... </Annotation>
       <Annotation> ... </Annotation>
       ...
   </Annotations>
 </document>


 Mark =
 // eine kommentierte Seite
 <Annotation>
   <key> upc </key>
   <imagefile> path to jpeg </imagefile>
   <geometry>  // list of wxDC::Draw*() objects painted over the image
   </geometry>
 </<Annotation>
 >

*/
#ifndef __DOCUMENTANNOTATIONS_H__
#define __DOCUMENTANNOTATIONS_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
//#include <wx/pen.h>
#include <map>
#include <vector>
#include "pdp11adapter.h"
#include "keywords.h"

// is pen drawn inside or outside around a box ?
enum class PenAlign { inside, outside };


// base class for all geometries
class DocumentPageAnnotationGeometry {
public:
    // construct subclass from xmlNode
    static DocumentPageAnnotationGeometry* load(wxXmlNode* xmlNode);
    // paint to wxDC of a scaledImage/Bitmap, and scale all coordinates by factors
    virtual void parse(std::string text) {
        UNREFERENCED_PARAMETER(text);
        wxLogError("parse() called for base class");
    }
    virtual std::string render() {
        wxLogError("render() called for base class");
        return "";
    }

    virtual void paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxPen* pen, enum PenAlign penAlign) {
        UNREFERENCED_PARAMETER(scaleX);
        UNREFERENCED_PARAMETER(scaleY);
        UNREFERENCED_PARAMETER(gc);
        UNREFERENCED_PARAMETER(pen);
        UNREFERENCED_PARAMETER(penAlign);
        wxLogError("paintScaled() called for base class");
    }
    virtual void dump() {
        wxLogError("dump() called for base class");
    }
};

// draw a rectangle (line width and color by global wxDC::pen)
class DocumentPageAnnotationRectangle : public DocumentPageAnnotationGeometry {
public:
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // 2 corners
    void parse(std::string text) override;
    std::string render() override;
    void paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxPen* pen, enum PenAlign penAlign) override;
    void dump() override;
};

// global pen
class DocumentPageAnnotationPolyLine : public DocumentPageAnnotationGeometry {
public:
    wxPoint2DDouble* points;
    unsigned pointCount;

    ~DocumentPageAnnotationPolyLine() {
        if (points)
            delete[] points;
    }
    void parse(std::string text) override;
    std::string render() override;
    void paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxPen* pen, enum PenAlign penAlign) override;
    void dump() override;
};

// global font
class DocumentPageAnnotationText : public DocumentPageAnnotationGeometry {
public:
    wxString text;
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // 2 corners of bounding box
    void parse(std::string text) override;
    std::string render() override;
    void paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxFont* font) override;
    void dump() override;
};

// a single annotation in a single document page,
// with all geometries to be painted over the image
class DocumentPageAnnotation {
private:
    // persistent helper wxImage with jpeg handler, causes memory leak?!
    //wxImage jpegImage;

public:
    // unique c-like identifer
    std::string key; // 11/34 uflow: 3 dighit octal micro program counter

    std::string displayLabel; // identifier as in original docs, free format

    // full path to jpg image, when it depends on key
    wxFileName imageFileName;

    std::vector<DocumentPageAnnotationGeometry*> geometries; // must be pointers, variable type
    // not free'd properly .. .doesnt care

    std::vector<std::string> dataFields;

    DocumentPageAnnotation();

    void parseGeometryFromNode(wxXmlNode* node);
    void parseDatafieldFromNode(wxXmlNode* node, unsigned idx);

    void parseFromNode(wxXmlNode* node, wxString xmlFileDirectory);

    // dc and size are that of a wxPanel,
    void paintScaledGeometries(wxGraphicsContext* gc, wxPen pen, PenAlign penAlign, double scaleX, double scaleY);
};

// all pages and possible annotations for a scanned DEC document
class DocumentPageAnnotations {

    // class tables for parsing
    static std::map<wxString, enum wxFontFamily> fontFamilyMap;
    static std::map<wxString, enum wxFontWeight> fontWeightMap;


private:
    // persistent helper wxImage, memory leak?
    //wxImage jpegImage;

    void parsePenFromNode(wxPen* pen, enum PenAlign* penAlign, wxXmlNode* node);
    void parseFontFromNode(wxFont* font, wxColour* color, wxXmlNode* node);


public:
    /*
    // what kind of DEC document is registered here?
    // corresponds to GUI notebook pages
    enum class Type { uflow, uword, datapath };
//    Type type; // not used?

    // pdp11 this docu is meant for
    Pdp11Adapter::Type pdp11Type; // not used ?
    */

    // full path to jpg image,
    // use when all annotations are paintend onto the same image
    wxFileName defaultImageFileName;


    // full path to special error page image
    wxFileName emptyPageFileName;

    // red half tranparent, width 30 for big unscaled 4000*2500 scans (
    const wxColour annotationPenColorDefault = wxColour(255, 0, 0, 127);// wxALPHA_OPAQUE);
    wxPen annotationPen = wxPen(annotationPenColorDefault, 30, wxPENSTYLE_SOLID);
    enum PenAlign annotationPenAlign = PenAlign::inside;

    wxFont annotationFont = wxFont(wxFontInfo(12).FaceName("Helvetica").Italic()); // to start with
    const wxColour annotationFontColorDefault = wxColour(255, 0, 0, 127);// wxALPHA_OPAQUE);
    wxColour annotationFontColor = annotationFontColorDefault;

    // key is a string. numerics must be formatted in a standard way to the key
    // eg: MPC as 3-digit octal
    std::map<std::string, DocumentPageAnnotation> pageAnnotations;

    DocumentPageAnnotations();

    void loadXml(wxFileName rootDir, std::string subDir, std::string xmlFileName);
    void dump();

    // search page image and annotation data
    DocumentPageAnnotation* find(std::string key);

    std::vector<std::string> getKeyList();

    wxGraphicsContext* paintScaledImage(wxWindow* window, wxFileName _imageFileName, double* scaleX, double* scaleY);

    // search currect page and show on DC
    void paintScaled(std::string key, wxWindow* window);
    void paintScaled(std::vector<std::string>& keyList, wxWindow* window);

    void paintEmptyPageScaled(wxWindow* window);

    void dumpKeywordsForDatafields(KeyPatternList& keyPatternList);

};


#endif // __DOCUMENTANNOTATIONS_H__

