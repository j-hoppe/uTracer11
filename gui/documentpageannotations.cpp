
/*
 DocumentPageAnnotations - helper to draw indexed geometries over an scaled document image

*/

#include "documentpageannotations.h"
#include <wx/graphics.h>
#include <sstream>
#include <vector>
#include "utils.h"
#include "keywords.h"


// pen width already scaled
// ! NO Error Window in OnPaint ... recursion!
void DocumentPageAnnotationRectangle::paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxPen* pen, enum PenAlign penAlign)
{
    wxCoord scaledX0 = round(scaleX * x0); // wxCoord is int
    wxCoord scaledY0 = round(scaleY * y0);
    wxCoord scaledWidth = round(scaleX * (x1 - x0));
    wxCoord scaledHeight = round(scaleY * (y1 - y0));

    int lineWidth = pen->GetWidth();
    switch (penAlign) {
    case PenAlign::inside:
        // draw rectangle into inner side of box
        scaledX0 += lineWidth / 2;
        scaledY0 += lineWidth / 2;
        scaledWidth -= lineWidth;
        scaledHeight -= lineWidth;
        break;
    case PenAlign::outside:
        // make rectangle lines surround given coordinates, not draw into inner space
        scaledX0 -= lineWidth / 2;
        scaledY0 -= lineWidth / 2;
        scaledWidth += lineWidth;
        scaledHeight += lineWidth;
        break;
    }

    gc->SetPen(*pen);
    gc->DrawRectangle(scaledX0, scaledY0, scaledWidth, scaledHeight);
}

// input: "x0,x1,y0,y1"
void DocumentPageAnnotationRectangle::parse(std::string text)
{
    // comma list to string vector
    std::stringstream ss(text);
    std::vector<std::string> list;
    while (ss.good()) {
        std::string substr;
        getline(ss, substr, ',');
        list.push_back(substr);
    }
    // val is x0,y0,x1,y1
    x0 = std::stoi(list[0]);
    y0 = std::stoi(list[1]);
    x1 = std::stoi(list[2]);
    y1 = std::stoi(list[3]);
}

std::string DocumentPageAnnotationRectangle::render()
{
    std::stringstream ss;
    ss << x0 << "," << y0 << "," << x1 << "," << y1;
    return ss.str();
}

void DocumentPageAnnotationRectangle::dump()
{
    wxLogInfo("rectangle(%s)", render());
}


// "x0,y0, x1,y1, ..."
void DocumentPageAnnotationPolyLine::parse(std::string text)
{
    // comma list to string vector
    std::stringstream ss(text);
    std::vector<std::string> list;
    while (ss.good()) {
        std::string substr;
        getline(ss, substr, ',');
        list.push_back(substr);
    }
    pointCount = list.size() / 2;
    if (pointCount < 2)
        wxLogError("PolyLine::parse(): line needs at least 2 points: %s", text);
    if (points)
        delete[] points;
    points = new wxPoint2DDouble[pointCount];
    for (size_t i = 0; i < pointCount; i++) {
        points[i].m_x = std::stoi(list[2 * i]);
        points[i].m_y = std::stoi(list[2 * i + 1]);
    }
}

std::string DocumentPageAnnotationPolyLine::render()
{
    std::stringstream ss;
    for (size_t i = 0; i < pointCount; i++) {
        if (i > 0)
            ss << ",";
        ss << points[i].m_x << "," << points[i].m_y;
    }
    return ss.str();
}



void DocumentPageAnnotationPolyLine::paintScaled(double scaleX, double scaleY, wxGraphicsContext* gc, wxPen* pen, PenAlign penAlign)
{
    UNREFERENCED_PARAMETER(penAlign);
    wxPoint2DDouble* scaledPoints;
    scaledPoints = new wxPoint2DDouble[pointCount];
    for (unsigned i = 0; i < pointCount; i++) {
        scaledPoints[i].m_x = points[i].m_x * scaleX;
        scaledPoints[i].m_y = points[i].m_y * scaleY;
    }
    gc->SetPen(*pen);
    // gc->DrawLines() draws a closed polygon under Ubuntu. On Win10 it's like wxDC.drawLines() not closing.
    gc->StrokeLines(pointCount, scaledPoints);
    delete[] scaledPoints;
}

void DocumentPageAnnotationPolyLine::dump()
{
    wxLogInfo("line(%s)", render());
}


DocumentPageAnnotation::DocumentPageAnnotation() {
}


// node is the <geometry>
void DocumentPageAnnotation::parseGeometryFromNode(wxXmlNode* node) {
    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString val = child->GetNodeContent().Trim(false).Trim(true);
        if (child->GetName().IsSameAs("rectangle", false)) {
            auto geom = new DocumentPageAnnotationRectangle();// not free'd on exit .. doesn't matter
            geom->parse(val.ToStdString());
            geometries.push_back(geom);
        }
        else if (child->GetName().IsSameAs("line", false)) {
            // val is x0,y0,x1,y1,...
            auto geom = new DocumentPageAnnotationPolyLine(); // not free'd on exit .. doesn't matter
            geom->parse(val.ToStdString());
            geometries.push_back(geom);
        }
        else
            wxLogWarning("Illegal geometry <%s>", child->GetName());

        child = child->GetNext();
    }
}

// node is the <datafield<1..5>>
void DocumentPageAnnotation::parseDatafieldFromNode(wxXmlNode* node, unsigned idx) {
    wxXmlNode* child = node->GetChildren();
    std::string multiLine;
    while (child) {
        // multiple <text> tags, join all als one string separated by \n
        if (child->GetName().IsSameAs("text", false)) {
            wxString val = child->GetNodeContent().Trim(false).Trim(true);
            if (multiLine.size() > 0) // line separator
                multiLine.append("\n");
            multiLine.append(val.ToStdString());
        }
        child = child->GetNext();
    }
    if (dataFields.size() <= idx)
        dataFields.resize(idx + 1);
    dataFields.at(idx) = multiLine; // save in vector
}

// node is the <Annotation>
void DocumentPageAnnotation::parseFromNode(wxXmlNode* node, wxString xmlFileDirectory)
{
    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString val = child->GetNodeContent().Trim(false).Trim(true);
        if (child->GetName().IsSameAs("key", false)) {
            key = val;
            if (displayLabel.size() == 0)
                displayLabel = val; // defaults to key
        }
        else if (child->GetName().IsSameAs("label", false)) {
            displayLabel = val;
        }
        else if (child->GetName().IsSameAs("imagefile", false)) {
            // filename. path relative to location of xml file
            wxString fullFilename = xmlFileDirectory + wxFileName::GetPathSeparator() + val;
            imageFileName = wxFileName(fullFilename);
        }
        else if (child->GetName().IsSameAs("geometry", false)) {
            // auto geometry = DocumentPageAnnotation();
            parseGeometryFromNode(child);
        }
        else if (child->GetName().IsSameAs("datafield1", false))
            parseDatafieldFromNode(child, 0);
        else if (child->GetName().IsSameAs("datafield2", false))
            parseDatafieldFromNode(child, 1);
        else if (child->GetName().IsSameAs("datafield3", false))
            parseDatafieldFromNode(child, 2);
        else if (child->GetName().IsSameAs("datafield4", false))
            parseDatafieldFromNode(child, 3);
        else if (child->GetName().IsSameAs("datafield5", false))
            parseDatafieldFromNode(child, 4);
        else if (child->GetName().IsSameAs("datafield6", false))
            parseDatafieldFromNode(child, 5);
        else if (child->GetName().IsSameAs("comment", false))
            ; // do not save, for now
        else
            wxLogWarning("Unknown node <%s>", child->GetName());

        child = child->GetNext();
    }
}


/*
// show document page image and paint annotation geometry with pen
// Conversion flow:
// jepgImage -> bitmap, graphicContext gc for wxPanel, bmp to gc, draw rect/lines onto gc
// adapts pen width
void DocumentPageAnnotation::paintScaled(wxWindow* window, wxPen pen) {
    wxImage jpegImage;
    jpegImage.AddHandler(new wxJPEGHandler);
    wxSize dcSize = window->GetSize();
    // load jpg file via wxImage, resize to dcSize, draw to dc via bitmap
    if (!jpegImage.LoadFile(imageFileName.GetAbsolutePath(), wxBITMAP_TYPE_JPEG)) {
        wxLogError("Can not load document page image from \"%s\"", imageFileName.GetAbsolutePath());
        return;
    }
    //    jpegImage.Clear();
        //return;
        // paint the document scan
    wxSize orgImageSize = jpegImage.GetSize();  // image size before rescale
    jpegImage.Rescale(dcSize.x, dcSize.y, wxIMAGE_QUALITY_HIGH); // BOX_AVERAGE on reduction
    auto bmp = wxBitmap(jpegImage); // wxImage->wxBitmap

    wxGraphicsContext* gc = wxGraphicsContext::Create(window);
    gc->DrawBitmap(bmp, 0, 0, bmp.GetSize().x, bmp.GetSize().y);

#ifdef DEMO
    gc->SetPen(*wxRED_PEN);
    wxGraphicsPath path = gc->CreatePath();
    path.AddCircle(500.0, 500.0, 500.0);
    path.MoveToPoint(0.0, 500.0);
    path.AddLineToPoint(1000.0, 500.0);
    path.MoveToPoint(500.0, 0.0);
    path.AddLineToPoint(500.0, 1000.0);
    path.CloseSubpath();
    path.AddRectangle(250.0, 250.0, 500.0, 500.0);
    gc->StrokePath(path);
#endif
    // paint the annotation geometries (rectangles, lines)
    // if dc is smaller than image, all geometries coordinates must be reduced
    double scaleX = (double)dcSize.x / orgImageSize.x;
    double scaleY = (double)dcSize.y / orgImageSize.y;
    double scaleLineWidth = std::max(scaleX, scaleY); // scale line width with image
    pen.SetWidth(pen.GetWidth() * scaleLineWidth);

    for (auto itGeom = geometries.begin(); itGeom != geometries.end(); itGeom++) {
        (*itGeom)->paintScaled(scaleX, scaleY, gc, &pen);
    }

    delete gc;
}
*/

// 2nd step: point geometries over the already painted image on an wxWindow
// one annotation can draw its geometries over the image of anoter annotation
// ! NO Error Window in OnPaint ... recursion!
void DocumentPageAnnotation::paintScaledGeometries(wxGraphicsContext* gc, wxPen pen, PenAlign penAlign, double scaleX, double scaleY) {
    double scaleLineWidth = std::max(scaleX, scaleY); // scale line width with image
    pen.SetWidth(pen.GetWidth() * scaleLineWidth);

    for (auto itGeom = geometries.begin(); itGeom != geometries.end(); itGeom++) {
        (*itGeom)->paintScaled(scaleX, scaleY, gc, &pen, penAlign);
    }
}

DocumentPageAnnotations::DocumentPageAnnotations() {
    //jpegImage.AddHandler(new wxJPEGHandler());
}

/* parse
  <pen>
    <border>inside|outside</border>
    <color>#00ff00</color>
    <width>30 </width>
    <alphapercent>25 </alphapercent>
  </pen>
*/
void DocumentPageAnnotations::parsePenFromNode(wxPen* pen, enum PenAlign* penAlign, wxXmlNode* node) {
    wxString colorText = "#000000";
    long width = 5;
    double alpha = 128; // 0..255
    *penAlign = PenAlign::inside;
    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString val = child->GetNodeContent().Trim(false).Trim(true);
        if (child->GetName().IsSameAs("align", false)) {
            if (val.IsSameAs("outside", false))
                *penAlign = PenAlign::outside;
        }
        else if (child->GetName().IsSameAs("color", false)) {
            colorText = val;
        }
        else if (child->GetName().IsSameAs("width", false)) {
            val.ToLong(&width);
        }
        else if (child->GetName().IsSameAs("alphapercent", false)) {
            val.ToDouble(&alpha);
            alpha = alpha * 255 / 100; // 0..100 -> 0..255
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
        }
        child = child->GetNext();
    }
    wxColour color(colorText);
    color = wxColour(color.Red(), color.Green(), color.Blue(), alpha);
    pen->SetColour(color);
    pen->SetWidth(width);
}

/* parse
  <font>
    <color>#00ff00</color>
    <family>Courier New</family>   //
    <weight>Courier New</weight>
    <pixelHeight>30 </pixelHeight>
    <alphapercent>25 </alphapercent>
  </font>
*/
// calss tables
std::map<wxString, enum wxFontFamily> DocumentPageAnnotations::fontFamilyMap = {
    { "DEFAULT", wxFONTFAMILY_DEFAULT },
    { "DECORATIVE", wxFONTFAMILY_DECORATIVE	},
    { "ROMAN ", wxFONTFAMILY_ROMAN  },
    { "SCRIPT", wxFONTFAMILY_SCRIPT	},
    { "SWISS", wxFONTFAMILY_SWISS  },
    { "MODERN", wxFONTFAMILY_MODERN	},
    { "TELETYPE", wxFONTFAMILY_TELETYPE	}
};

std::map<wxString, enum wxFontWeight> DocumentPageAnnotations::fontWeightMap = {
    { "THIN", wxFONTWEIGHT_THIN  },
    { "EXTRALIGHT", wxFONTWEIGHT_EXTRALIGHT },
    { "LIGHT", wxFONTWEIGHT_LIGHT },
    { "NORMAL", wxFONTWEIGHT_NORMAL },
    { "MEDIUM", wxFONTWEIGHT_MEDIUM },
    { "SEMIBOLD", wxFONTWEIGHT_SEMIBOLD },
    { "BOLD", wxFONTWEIGHT_BOLD },
    { "EXTRABOLD", wxFONTWEIGHT_EXTRABOLD },
    { "HEAVY", wxFONTWEIGHT_HEAVY },
    { "EXTRAHEAVY", wxFONTWEIGHT_EXTRAHEAVY }
};


void DocumentPageAnnotations::parseFontFromNode(wxFont* font, wxColour* color, wxXmlNode* node) {
    wxString colorText = "#000000";
    // implement only some properties for now
    wxString familyText = "SWISS"; // see enum wxFontFamily
    wxString weightText = "NORMAL"; // see enum wxFontWeight
    //int	pixelHeight = 11 ; // height of chars in pixel
    double alpha = 128; // 0..255
    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString val = child->GetNodeContent().Trim(false).Trim(true);
        if (child->GetName().IsSameAs("color", false)) {
            colorText = val;
        }
        else if (child->GetName().IsSameAs("family", false)) {
            familyText = val.MakeUpper(); // map key
        }
        else if (child->GetName().IsSameAs("weight", false)) {
            weightText = val.MakeUpper(); // map key
        }
        else if (child->GetName().IsSameAs("alphapercent", false)) {
            val.ToDouble(&alpha);
            alpha = alpha * 255 / 100; // 0..100 -> 0..255
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;
        }
        child = child->GetNext();
    }
    *color = wxColour(colorText);
    *color = wxColour(color->Red(), color->Green(), color->Blue(), alpha);

    enum wxFontFamily family = wxFONTFAMILY_DEFAULT;
    if (fontFamilyMap.count(familyText) == 0)
        wxLogError("Unknown font family \"%s\"", familyText);
    else
        family = fontFamilyMap.at(familyText);
    enum wxFontWeight weight = wxFONTWEIGHT_NORMAL;
    if (fontWeightMap.count(weightText) == 0)
        wxLogError("Unknown font weight \"%s\"", weightText);
    else
        weight = fontWeightMap.at(weightText);
    font->SetFamily(family);
    font->SetWeight(weight);
}



// call like "...resources\pdp1134", "MP00082", "filename.xml"
void DocumentPageAnnotations::loadXml(wxFileName resourcePath, std::string subDir, std::string xmlFileName)
{
    wxFileName path = resourcePath; // to be extended

    // first, fix link to empty page image
    // ist ...\resources\pdp1134\..\empty-document-page.jpg
    emptyPageFileName = resourcePath;
    emptyPageFileName.AppendDir("..");
    emptyPageFileName.SetFullName("empty-document-page.jpg");

    // append components to path
    //subDir += wxFileName::GetPathSeparator();
    path.SetFullName(xmlFileName);
    path.AppendDir(subDir);
    // now .../resource/subdir/filename.xml

    wxXmlDocument doc;
    if (!doc.Load(path.GetAbsolutePath())) {
        auto errormsg = wxString::Format(" XML file %s could not be opened", path.GetAbsolutePath());
        wxMessageBox(errormsg, "File error", wxICON_ERROR);
        wxLogError(errormsg);// gui not yet open
        abort();
        return;
    }
    wxLogInfo("%s opened", path.GetAbsolutePath());

    // iterate all <Annotation>
    wxXmlNode* node = doc.GetRoot()->GetChildren();
    while (node) {
        if (node->GetName().IsSameAs("imagefile", false)) {
            // global filename. path relative to location of xml file
            auto val = node->GetNodeContent().Trim(false).Trim(true);
            wxString fullFilename = path.GetPath() + wxFileName::GetPathSeparator() + val;
            defaultImageFileName = wxFileName(fullFilename);
        }
        else if (node->GetName().IsSameAs("pen", false)) {
            parsePenFromNode(&annotationPen, &annotationPenAlign, node);
            //wxLogDebug("node->GetName()=%s", node->GetName());
        }
        else if (node->GetName().IsSameAs("font", false)) {
            parseFontFromNode(&annotationFont, &annotationFontColor, node);
            //wxLogDebug("node->GetName()=%s", node->GetName());
        }
        else if (node->GetName().IsSameAs("annotation", false)) {
            auto dpa = DocumentPageAnnotation();
            dpa.parseFromNode(node, path.GetPath()); // dir of xml file
            pageAnnotations[dpa.key] = dpa; // allocate & store
        }
        node = node->GetNext();
    }
    // dump();
}

void DocumentPageAnnotations::dump() {
    // query all keys in insert-order
    for (auto itpa = pageAnnotations.begin(); itpa != pageAnnotations.end(); itpa++) {
        wxLogInfo("key=%s", itpa->first.c_str());
        DocumentPageAnnotation* pa = &(itpa->second);
        for (auto itgeom = pa->geometries.begin(); itgeom != pa->geometries.end(); itgeom++)
            (*itgeom)->dump();
    }
}


DocumentPageAnnotation* DocumentPageAnnotations::find(std::string key) {
    auto dpaIt = pageAnnotations.find(key); // is iterator
    if (dpaIt == pageAnnotations.end())
        return nullptr;
    else
        return &dpaIt->second;
}

// return all annotain keys as list
std::vector<std::string> DocumentPageAnnotations::getKeyList() {
    std::vector<std::string> result;
    for (auto itpa = pageAnnotations.begin(); itpa != pageAnnotations.end(); itpa++) {
        DocumentPageAnnotation* pa = &(itpa->second);
        result.push_back(pa->key);
    }
    return result;
}

// 1st step: paint the image onto a wxWindow
// defaultImageFileName is used if no "per-annotation" image is given
// return stretch ratio anf graphic conetxt for further painting
// ! NO Error Window in OnPaint ... recursion!
wxGraphicsContext* DocumentPageAnnotations::paintScaledImage(wxWindow* window, wxFileName imageFileName, double* scaleX, double* scaleY) {
    wxImage jpegImage;
    //jpegImage.AddHandler(new wxJPEGHandler);
    wxSize dcSize = window->GetSize();
    // load jpg file via wxImage, resize to dcSize, draw to dc via bitmap
    if (!jpegImage.LoadFile(imageFileName.GetAbsolutePath(), wxBITMAP_TYPE_JPEG)) {
        wxLogFatalError("Can not load document page image from \"%s\"", imageFileName.GetAbsolutePath());
        return nullptr;
    }
    //    jpegImage.Clear();
    //return;
    // paint the document scan
    wxSize orgImageSize = jpegImage.GetSize();  // image size before rescale
    jpegImage.Rescale(dcSize.x, dcSize.y, wxIMAGE_QUALITY_HIGH); // BOX_AVERAGE on reduction
    auto bmp = wxBitmap(jpegImage); // wxImage->wxBitmap

    wxGraphicsContext* gc = wxGraphicsContext::Create(window);
    gc->DrawBitmap(bmp, 0, 0, bmp.GetSize().x, bmp.GetSize().y);

    // paint the annotation geometries (rectangles, lines)
    // if dc is smaller than image, all geometries coordinates must be reduced
    *scaleX = (double)dcSize.x / orgImageSize.x;
    *scaleY = (double)dcSize.y / orgImageSize.y;
    return gc;
}

// paint the document page onto a wxPanel, mark with a single annotation
// error window if not found
// default page if key not found
// if per-annotation filename empty: use global default
void DocumentPageAnnotations::paintScaled(std::string key, wxWindow* window) {
    if (!window->IsShownOnScreen())
        return; // do not paint on hidden panels

    // wxLogDebug("DocumentPageAnnotations::paintScaled(%s)", key);
    auto dpa = find(key);
    if (dpa == nullptr)
        paintEmptyPageScaled(window); // no page found for this key
    else {
        double scaleX, scaleY;
        wxFileName imageFileName = dpa->imageFileName; // try per annotation first
        if (!imageFileName.IsOk())
            imageFileName = defaultImageFileName;
        if (!imageFileName.IsOk())
            wxLogFatalError("paintScaled(keylist): annotation image not defined in XML");
        wxGraphicsContext* gc = paintScaledImage(window, imageFileName, &scaleX, &scaleY);
        if (gc != nullptr) {
            dpa->paintScaledGeometries(gc, annotationPen, annotationPenAlign, scaleX, scaleY);
            delete gc;
        }
    }
}

// paint the document page onto a wxPanel, mark with a multiple annotations
// all annotations are painted on the same global imageFileName.
//    (annotation.imageFileName is ignored)
// ! NO Error Window in OnPaint ... recursion!
void DocumentPageAnnotations::paintScaled(std::vector<std::string>& keyList, wxWindow* window) {
    if (!window->IsShownOnScreen())
        return; // do not paint on hidden panels

    // wxLogDebug("DocumentPageAnnotations::paintScaled(%s)", stringVectorAsCommaList(keyList, ","));
    double scaleX = 1, scaleY = 1;
    // 1) paint background page image
    if (!defaultImageFileName.IsOk())
        wxLogFatalError("paintScaled(keylist): default image not defined in XML");

    wxGraphicsContext* gc = paintScaledImage(window, defaultImageFileName, &scaleX, &scaleY);
    if (gc == nullptr)
        return;

    // 2. paint geometries of annotations
    for (auto itKey = keyList.begin(); itKey != keyList.end(); itKey++) {
        auto dpa = find(*itKey);
        assert(dpa != nullptr); // already checked: key points to valid annotation
        // wxLogDebug("paintScaled(keylist): painting key %s, label=%s", *itKey, dpa->displayLabel);
        dpa->paintScaledGeometries(gc, annotationPen, annotationPenAlign, scaleX, scaleY);
    }
    //#endif
    delete gc;
}


// display a notifcation image when there's no document page for a micro program counter key
// ! NO Error Window in OnPaint ... recursion!
void DocumentPageAnnotations::paintEmptyPageScaled(wxWindow* window) {
    wxImage jpegImage;
    jpegImage.AddHandler(new wxJPEGHandler);
    wxSize dcSize = window->GetSize();
    if (!jpegImage.LoadFile(emptyPageFileName.GetAbsolutePath(), wxBITMAP_TYPE_JPEG)) {
        wxLogFatalError("Can not load empty page image from \"%s\"", emptyPageFileName.GetAbsolutePath());
        return;
    }
    jpegImage.Rescale(dcSize.x, dcSize.y, wxIMAGE_QUALITY_NORMAL);
    auto bmp = wxBitmap(jpegImage); // wxImage->wxBitmap
    wxClientDC dc(window);
    dc.DrawBitmap(bmp, 0, 0);
}

// output for every datafield the matching keys
void DocumentPageAnnotations::dumpKeywordsForDatafields(KeyPatternList& keyPatternList) {
    // for all annotations, and all datafields:
    for (auto itDpa = pageAnnotations.begin(); itDpa != pageAnnotations.end(); itDpa++) {
        std::string key = itDpa->first;
        DocumentPageAnnotation* dpa = &itDpa->second;
        // all datafields
        for (unsigned dataFieldIdx = 0; dataFieldIdx < dpa->dataFields.size(); dataFieldIdx++) {
            bool anyRegexForThisField = false;
            // apply all regexes. list each matching regex, and finally the set of produced keys
            std::string dataField = dpa->dataFields[dataFieldIdx];
            std::string dataFieldPrintable = dataField;
            //  make '\n' visible with ANSI paragraph symbol, avoid represention issues on eclipse
            std::replace(dataFieldPrintable.begin(), dataFieldPrintable.end(), '\n', '\xb6');
            //std::replace(dataFieldPrintable.begin(), dataFieldPrintable.end(), '\n', '�'); //  make '\n' visible
            for (auto itKeyPattern = keyPatternList.keyPatterns.begin(); itKeyPattern != keyPatternList.keyPatterns.end(); itKeyPattern++)
                if (itKeyPattern->dataFieldIdx == dataFieldIdx) {
                    // one iteration for each regex which a a pattern for this datafield
                    std::vector<std::string> resultKeyList;
                    if (itKeyPattern->addKeysToListByMatch(dataField, resultKeyList) == 0)
                        continue; // no match
                    if (!anyRegexForThisField) {// match! header?
                        //wxLogInfo("key=%s, dataField[%d]= \"%s\": matching regexes", key, dataFieldIdx, dataFieldPrintable);
                        anyRegexForThisField = true;
                    }
                    // wxLogInfo("    regex => keys: \"%s\" => %s", itKeyPattern->regexText, stringVectorAsCommaList(resultKeyList));
                }
            // Print summary for each datafield
            if (!anyRegexForThisField)
                wxLogDebug("key=%s, dataField[%d]= \"%s\" : NO MATCHING REGEXES", key, dataFieldIdx, dataFieldPrintable);
        }
    }
}

