/** @file
  Entrypoint of "http" shell standalone application.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved. <BR>
  Copyright (c) 2015, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2020, Broadcom. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Http.h"

HTTP_DOWNLOAD_PROGRESS_CALLBACK gHttpDownloadProgressCallback = NULL;

EFI_STATUS
EFIAPI
HttpDownloadFile (
  IN  CHAR16                           *Url,
  OUT CHAR8                            **ResponseBuffer,
  OUT UINTN                            *ResponseSize,
  IN  HTTP_DOWNLOAD_PROGRESS_CALLBACK  ProgressCallback OPTIONAL
  )
{
  EFI_STATUS  Status;
  if (ProgressCallback != NULL) {
    gHttpDownloadProgressCallback = ProgressCallback;
  }
  Status = RunHttp (Url, NULL, NULL, 0, 0, ResponseBuffer, ResponseSize);
  DEBUG ((DEBUG_INFO, "HttpDownloadFile() RunHttp return %r\n", Status));
  return Status;
}

