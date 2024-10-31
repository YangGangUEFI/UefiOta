/** @file
  Header file for 'http' command functions.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved. <BR>
  Copyright (c) 2015, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2020, Broadcom. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HTTP_DOWNLOAD_LIB_HTTP_H_
#define _HTTP_DOWNLOAD_LIB_HTTP_H_

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HttpLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NetLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/HttpUtilities.h>
#include <Protocol/ServiceBinding.h>

#include <Library/HttpDownloadLib.h>

#define LIB_FREE_NON_NULL(Pointer)  \
  do {                                \
    if ((Pointer) != NULL) {          \
      FreePool((Pointer));            \
      (Pointer) = NULL;               \
    }                                 \
  } while(FALSE)

#define HTTP_APP_NAME  L"http"

#define REQ_OK           0
#define REQ_NEED_REPEAT  1

//
// Download Flags.
//
#define DL_FLAG_TIME      BIT0 // Show elapsed time.
#define DL_FLAG_KEEP_BAD  BIT1 // Keep files even if download failed.

typedef struct {
  UINTN                   ContentDownloaded;
  UINTN                   ContentLength;
  UINTN                   LastReportedNbOfBytes;
  UINTN                   BufferSize;
  UINTN                   Status;
  UINTN                   Flags;
  UINT8                   *Buffer;
  CHAR16                  *ServerAddrAndProto;
  CHAR16                  *Uri;
  EFI_HTTP_TOKEN          ResponseToken;
  EFI_HTTP_TOKEN          RequestToken;
  EFI_HTTP_PROTOCOL       *Http;
  EFI_HTTP_CONFIG_DATA    HttpConfigData;
} HTTP_DOWNLOAD_CONTEXT;

/**
  Function for 'http' command.

  @param[in] DownloadUrl        Url like http://example.com/example.
  @param[in] NicNameIn          Specific NIC name like "eth0".
  @param[in] LocalPortIn        LocalPort for TCP connect, Decimal.
  @param[in] BufferSizeIn       Specific BufferSize.
  @param[in] TimeOutMillisecIn  Specific timeout value in millsecond, 0 means auto.

  @retval  SHELL_SUCCESS            The 'http' command completed successfully.
  @retval  SHELL_ABORTED            The Shell Library initialization failed.
  @retval  SHELL_INVALID_PARAMETER  At least one of the command's arguments is
                                    not valid.
  @retval  SHELL_OUT_OF_RESOURCES   A memory allocation failed.
  @retval  SHELL_NOT_FOUND          Network Interface Card not found.
  @retval  SHELL_UNSUPPORTED        Command was valid, but the server returned
                                    a status code indicating some error.
                                    Examine the file requested for error body.
**/
EFI_STATUS
EFIAPI
RunHttp (
  IN  CHAR16  *DownloadUrl,
  IN  CHAR16  *NicNameIn,        OPTIONAL
  IN  CHAR16  *LocalPortIn,      OPTIONAL
  IN  UINTN   BufferSizeIn,      OPTIONAL
  IN  UINT32  TimeOutMillisecIn, OPTIONAL
  OUT CHAR8   **DownloadedBuffer,
  OUT UINTN   *DownloadedBufferSize
  );

#endif // _HTTP_DOWNLOAD_LIB_HTTP_H_
