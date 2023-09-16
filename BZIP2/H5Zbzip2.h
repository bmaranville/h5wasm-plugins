#include "H5PLextern.h"
#include <hdf5.h>

#ifndef __H5ZBZIP2_H__
#define __H5ZBZIP2_H__ 1

#define FILTER_BZIP2 307

int register_bzip2(char **version, char **date);
size_t bzip2_deflate(unsigned int flags, size_t cd_nelmts,
                     const unsigned int cd_values[], size_t nbytes,
                     size_t *buf_size, void **buf);

H5_DLLVAR H5Z_class_t H5Z_BZIP2[1] = {{
    H5Z_CLASS_T_VERS,             /* H5Z_class_t version */
    (H5Z_filter_t)(FILTER_BZIP2), /* filter_id */
    1, 1,                         /* Encoding and decoding enabled */
    "bzip2",                      /* comment */
    NULL,                         /* can_apply_func */
    NULL,                         /* set_local_func */
    (H5Z_func_t)(bzip2_deflate)   /* filter_func */
  }};

H5PL_type_t H5PLget_plugin_type(void) {return H5PL_TYPE_FILTER;}
const void *H5PLget_plugin_info(void) {return H5Z_BZIP2;}

#endif /* ! defined __H5ZBZIP2_H__ */
