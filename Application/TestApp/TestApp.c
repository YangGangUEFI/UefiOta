/** @file
  This sample application bases on HelloWorld PCD setting
  to print "UEFI Hello World!" to the UEFI Console.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HttpDownloadLib.h>



VOID 
HttpDownloadFileProgress (
  IN CHAR16  *Str
  )
{
  if (Str != NULL) {
    CreatePopUp (
      EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
      NULL,
      Str,
      NULL
      );
  }
}


VOID
BiosUpdateCheckHttp()
{
  EFI_STATUS     Status;
  CHAR8          *DownloadBuffer = NULL;
  UINTN          DownloadSize = 0;
  CHAR8          *SearchResult;
  CHAR8          *MessageStr;
  CHAR8          *BiosBinLinkStr;
  UINTN          StringSize1, StringSize2;
  CHAR16         *BiosLink, *NewMessage;
  CHAR8          *TempBuffer;
  EFI_INPUT_KEY  Key;

  //
  // Check /update
  //
  // if success, return:
  // {
  //   "message": "New BIOS version available: V1R17",
  //   "image_url": "http://192.168.10.23:5000/BIOS.bin"
  // }
  //
  Status = HttpDownloadFile (L"http://192.168.10.23:5000/update", &DownloadBuffer, &DownloadSize, NULL);
  if (!EFI_ERROR(Status) && DownloadBuffer != NULL) {
    TempBuffer = AllocateZeroPool (DownloadSize + 1);
    CopyMem (TempBuffer, DownloadBuffer, DownloadSize);
    FreePool (DownloadBuffer);
    DownloadBuffer = NULL;
    DEBUG ((DEBUG_INFO, "%a\n", TempBuffer));

    SearchResult = AsciiStrStr (TempBuffer, ", ");
    if (SearchResult != NULL) {
      StringSize1 = DownloadSize - 13 - AsciiStrLen (SearchResult) - 1;
      MessageStr = AllocateZeroPool (StringSize1 + 1);
      CopyMem (MessageStr, TempBuffer+13, StringSize1);

      DEBUG ((DEBUG_INFO, "%a\n", MessageStr));

      StringSize2 = DownloadSize - 13 - StringSize1 - 17 - 2;
      BiosBinLinkStr = AllocateZeroPool (StringSize2 + 1);
      CopyMem (BiosBinLinkStr, TempBuffer+13+StringSize1+17, StringSize2);
      DEBUG ((DEBUG_INFO, "%a\n", BiosBinLinkStr));

      if (TempBuffer != NULL) FreePool (TempBuffer);
      TempBuffer = NULL;
      DownloadSize = 0;

      BiosLink = AllocateZeroPool ((AsciiStrLen (BiosBinLinkStr) + 1) * sizeof (CHAR16));
      AsciiStrToUnicodeStrS (BiosBinLinkStr, BiosLink, (AsciiStrLen (BiosBinLinkStr) + 1) * sizeof (CHAR16));

      NewMessage = AllocateZeroPool ((AsciiStrLen (MessageStr) + 1) * sizeof (CHAR16));
      AsciiStrToUnicodeStrS (MessageStr, NewMessage, (AsciiStrLen (MessageStr) + 1) * sizeof (CHAR16));

      FreePool (MessageStr);
      FreePool (BiosBinLinkStr);

      do {
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          NewMessage,
          BiosLink,
          L"Press ENTER to continue update, Press ESC to cancel update",
          NULL
          );
      } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

      FreePool (NewMessage);

      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        Status = HttpDownloadFile (BiosLink, &DownloadBuffer, &DownloadSize, HttpDownloadFileProgress);
        FreePool (BiosLink);
        DEBUG ((DEBUG_INFO, "DownloadSize: 0x%x\n", DownloadSize));

        //
        // Call BIOS Update API!!!
        //

        if (DownloadBuffer != NULL) FreePool (DownloadBuffer);
        DownloadBuffer = NULL;
        DownloadSize = 0;
      } else {
        return;
      }
    }
  } else {
    do {
      CreatePopUp (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        L"No BIOS Update Detected!",
        L"Press ESC to exit",
        NULL
        );
    } while (Key.ScanCode != SCAN_ESC);
  }
}


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  BiosUpdateCheckHttp();

  return EFI_SUCCESS;
}
