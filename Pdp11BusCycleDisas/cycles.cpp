
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "utils.hpp"
#include "cycles.hpp"



/********************************************************
 *	print a unibus address, even with missing lmsb bits
 * an adress has 18 bits and is printed octal, so 6 digits width
 *	*/
char	*pdp11bus_addr_astext(unsigned addr, unsigned lsb_bits)
{
    static char buff[256];
    char buff1[256];
    if (lsb_bits == 0)
        sprintf_s(buff, sizeof(buff), "%06o", addr);
    else {
        // unknown digits: prefix 0, as "?"
        sprintf_s(buff1, sizeof(buff1), "?%0*o", (lsb_bits + 2) / 3, addr);
        sprintf_s(buff, sizeof(buff), "%6s", buff1); // left fill with spaces to 6 digits
    }

    return buff;
}





/********************************************************
 * reset all fields
*/
void CycleList::clear(int max_pdp11bus_cycle_count)
{
    for (int i = 0; i < max_pdp11bus_cycle_count; i++) {
        pdp11bus_cycle_t	*bc = get(i);
        bc->bus_address.iopage = 0;
        bc->bus_address.val = 0;
        bc->bus_address_lsb_bit_count = 0;
        bc->bus_cycle = 0;
        bc->bus_data = 0;
        bc->tag = 0;
        bc->timestamp = 0;
        bc->disas_class = DISAS_CYCLE_CLASS_UNKNOWN;
        strcpy_s(bc->disas_opcode, MAX_OPCODE_STRING_LEN, "");
        strcpy_s(bc->disas_operands, MAX_OPERAND_STRING_LEN, "");
        strcpy_s(bc->disas_comment, MAX_INFO_STRING_LEN, "");
        strcpy_s(bc->disas_address_info, MAX_INFO_STRING_LEN, "");
        strcpy_s(bc->disas_debug_info, MAX_INFO_STRING_LEN, "");
    }
    pdp11bus_cycles_count = 0;
    cur_idx = 0;
}




/**************************************
 *	endmark: cycle list can be truncated by setting
 *	and "end mark".
 */
void	CycleList::cycle_set_endmark(int idx)
{
    pdp11bus_cycle_t *bc = get(idx);
    bc->disas_class = DISAS_CYCLE_CLASS_END;
}

int		CycleList::cycle_is_endmark(int idx) {
    pdp11bus_cycle_t *bc = get(idx);
    return (bc->disas_class == DISAS_CYCLE_CLASS_END);
}




/********************************************************
 fetch adddr and data from test input text file

  Input file is snapshot of unibus trace by Unilizer

  // Evaluated UNIBUS logic analyzer sample from 08.04.14 19:02
 // (UniLizer input file was E:\temp\unibus.CSV)
  #  Timestamp [µs]  MSYN  SSYN  Cycle    Addr    Data
  1      37.586,350     1     1   DATI  765022  005203    ; 0x3EA12 0x0A83
  2      37.589,100     1     1   DATI  765024  005103    ; 0x3EA14 0x0A43
  3      37.591,900     1     1   DATI  765026  006203    ; 0x3EA16 0x0C83
  ....
 70      37.778,450     1     0  DATOB  777566  000012    ; 0x3FF76 0x000A
 ...
 85      38.774,950     1     1   DATI  777707  165520    ; 0x3FFC7 0xEB50

 result:
 -1 = failure/EOF,
  0 = line skipped
  1 = ok
*/
int	cycle_read(FILE *f, pdp11bus_cycle_t *bc) {
    char	buff[1024];
    char *s, *r, *w;
    char	str_tag[1024], str_timestamp[1024], str_duration[1024], str_control[1024], str_addr[1024], str_data[1024];
    int n;

    if (!fgets(buff, sizeof(buff), f))
        return -1;

    s = buff;
    // skip leading sapce
    while (*s && isspace(*s))
        s++;

    if (!strlen(s))
        return 0;

    // filter out comment lines
    if (!isdigit(s[0]))
        return 0;

    // Sample #;Time [µs];Duration [ns];MSYN;SSYN;C1,C0;IOPage;Addr (octal);Addr (hex);Data (octal);Data (hex)
    // 2;-18.873,870;20;0;0;0;-;176500;0FD40;000000;0000
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
    n = sscanf(s, "%s;%s;%s;%s;%s;%s",
        str_tag, str_timestamp, str_duration, str_control, str_addr, str_data);
    if (n != 6)
        return 0;

    /*
    // 85      38.774,950    DATI  777707  165520    ; 0x3FFC7 0xEB50
    n = sscanf_s(s, "%s %s %s %s %s",
                 str_tag, sizeof(str_tag), str_timestamp, sizeof(str_timestamp),
                 str_control, sizeof(str_control), str_addr, sizeof(str_addr), str_data, sizeof(str_data));
    if (n != 5)
        return 0 ;
    */
    // printf("%s %s %s\n", str_control, str_addr, str_data) ;

    bc->bus_cycle = pdp11_buscontrol_txt2val(str_control);

    if (bc->bus_cycle == BUSCYCLE_NONE)
        return 0; // unknown control code

    // convert decimal tag (sample number)
    n = sscanf(str_tag, "%u", &(bc->tag));
    if (n != 1)
        return 0;

    // timestamp in µs in the form  "37.591,900"
    // remove . and , from s_timestamp => nano seconds!
    // r= read, w = write ;
    for (r = w = str_timestamp; *r; r++)
        if (*r != '.' && *r != ',')
            *w++ = *r;
    *w = 0; // terminate
#ifdef _MSC_VER
    n = sscanf(str_timestamp, "%I64u", &(bc->timestamp));
#else
    n = sscanf(str_timestamp, "%lu", &(bc->timestamp));
#endif
    if (n != 1)
        return 0;

    // Address: convert (octal) numbers to integers
    // may have leading "?" to indicte missng address lsb bits
    n = sscanf(str_addr, "%o", &(bc->bus_address.val));
    if (n == 1) { // all bits given
        bc->bus_address_lsb_bit_count = 0;
    }
    else {
        n = sscanf(str_addr, "?%o", &(bc->bus_address.val));
        if (n == 1) {
            // count digits
            bc->bus_address_lsb_bit_count = 3 * ((unsigned)strlen(str_addr) - 1); // subtract one '?'
        }
        else
            return 0;
    }

    // Data: convert (octal) numbers to integers
    n = sscanf(str_data, "%o", &(bc->bus_data));
    if (n != 1)
        return 0;

    return 1;
}




// dump the list
void CycleList::dump(FILE *f, bool with_disas_fields)
{
    int idx;
    if (!with_disas_fields) {
        for (idx = 0; idx < size(); idx++) {
            pdp11bus_cycle_t *bc = get(idx);
#ifdef _MSC_VER
            const char* fmt = "tag=%4u  timstamp_ns=%9I64u  C1,0=%o  addr=%s  data=%6o\n";
#else
            const char* fmt = "tag=%4u  timstamp_ns=%9lu  C1,0=%o  addr=%s  data=%6o\n";
#endif
            fprintf(f, fmt, bc->tag, bc->timestamp, bc->bus_cycle,
                pdp11bus_addr_astext(bc->bus_address.val, bc->bus_address_lsb_bit_count),
                bc->bus_data);
        }
    }
    else {
        fprintf(f, "%4s  %9s  %5s  %6s %6s  %8s  %16s  %-s\n",
            "tag", "time[ns]", "C1,0", "Addr", "Data", "Opcode", "Operands", "Comment");
        for (idx = 0; idx < size(); idx++) {
            pdp11bus_cycle_t *bc = get(idx);
            if (cycle_is_endmark(idx))
                break; // stop dump
#ifdef _MSC_VER
            const char* fmt = "%4u  %9I64u  %5s  %6s %6o  %8s  %16s  ; %s%s%s\n";
#else
            const char* fmt = "%4u  %9lu  %5s  %6s %6o  %8s  %16s  ; %s%s%s\n";
#endif
            fprintf(f, fmt, bc->tag, bc->timestamp,
                pdp11_buscontrol_val2txt(bc->bus_cycle),
                pdp11bus_addr_astext(bc->bus_address.val, bc->bus_address_lsb_bit_count),
                bc->bus_data, bc->disas_opcode, bc->disas_operands, bc->disas_comment,
                strlen(bc->disas_address_info) ? ", address = " : "",
                bc->disas_address_info
            );
        }
    }
}
