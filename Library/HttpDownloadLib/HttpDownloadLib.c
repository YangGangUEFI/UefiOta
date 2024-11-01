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
  IN OUT UINTN                         *BufferSize,
  IN     VOID                          *Buffer          OPTIONAL,
  IN  HTTP_DOWNLOAD_PROGRESS_CALLBACK  ProgressCallback OPTIONAL
  )
{
  EFI_STATUS  Status;

  if ((*BufferSize != 0) && (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ProgressCallback != NULL) {
    gHttpDownloadProgressCallback = ProgressCallback;
  }

  Status = RunHttp (Url, NULL, NULL, 0, 0, BufferSize, Buffer);
  DEBUG ((DEBUG_INFO, "HttpDownloadFile() RunHttp return %r\n", Status));
  if (Status == EFI_BUFFER_TOO_SMALL) {
    DEBUG ((DEBUG_INFO, "HttpDownloadFile() need 0x%x bytes buffer\n", *BufferSize));
  }
  return Status;
}

