#ifndef INITGUID
#define INITGUID
#endif
#include <objbase.h>

/*
 * GUIDs are made from applying the SHA256 function on ASCII object names and
 * then cutting of the lower half of the resulting hash value for the GUID.
 * For IcedTea-Web MSI packages, the same scheme of genesis for GUIDs applies,
 * except for GUIDs in the ComponentId column of the Component table.
 *
 * Package Code:
 * IcedTea.IcedTea-Web                   {FC23E8B6-BA9D-273C-02FA-F38D77CDCEAE}
 * ProductCode:
 * IcedTea.IcedTea-Web                   {FC23E8B6-BA9D-273C-02FA-F38D77CDCEAE}
 * UpgradeCode:
 * IcedTea.IcedTea-Web.1.6.1.0           {28D0A3BC-A312-AF61-0A77-5A7FC95E28FC}
 */

/**
 * <summary>IcedTea-Web class ID.</summary>
 * <para>
 * This is effectively <c>SHA256("CLSID_IcedTea_Web")[15,31]</c>.
 * </para>
 * TODO: Check if IcedTea-Web's CLSID should be equal to Oracle's Java Web
 * Start.
 */
DEFINE_GUID(CLSID_IcedTea_Web, 0x3E05C690, 0xD2D8, 0x1742,
            0xBC, 0x50, 0xB6, 0xB7, 0x92, 0xE2, 0xBB, 0xB0);
// const CLSID CLSID_IcedTea_Web = {
  // 0x3E05C690, 0xD2D8, 0x1742, {
    // 0xBC, 0x50,  0xB6, 0xB7, 0x92, 0xE2, 0xBB, 0xB0
  // }
// };

/**
 * <summary>IcedTea-Web interface ID.</summary>
 * <para>
 * This is effectively <c>SHA256("IID_IcedTea_Web")[15,31]</c>.
 * </para>
 * TODO: Check if IcedTea-Web's IID should be equal to Oracle's Java Web Start.
 */
DEFINE_GUID(IID_IIcedTea_Web, 0x277E419C, 0x66E2, 0x5D44,
            0x85, 0x86, 0xFD, 0xEA, 0x2C, 0x1A, 0xFB, 0x29);
 // const IID IID_IIcedTea_Web = {
  // 0x0564f63a, 0x4e6f, 0xf082, {
    // 0x75, 0x4d, 0xda, 0xe2, 0x13, 0x51, 0x4d, 0xd4
  // }
// };

/**
 * <summary>IcedTea-Web shell extension interface ID.</summary>
 * <para>
 * This is effectively <c>SHA256("IID_IcedTea_WebShellExtension")[15,31]</c>.
 * </para>
 * <seealso cref="CLSID_IcedTea_Web"/>
 */
DEFINE_GUID(IID_IIcedTea_WebShellExtension, 0xBFAE362F, 0x24AB, 0x890E,
            0x97, 0xEB, 0xD7, 0x54, 0x42, 0x61, 0x5C, 0xFC);
