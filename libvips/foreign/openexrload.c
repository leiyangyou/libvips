/* load openexr from a file
 *
 * 5/12/11
 * 	- from openslideload.c
 */

/*

    This file is part of VIPS.
    
    VIPS is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/*
#define DEBUG
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /*HAVE_CONFIG_H*/
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/buf.h>
#include <vips/internal.h>

#include "openexr2vips.h"

typedef struct _VipsForeignLoadOpenexr {
	VipsForeignLoad parent_object;

	/* Filename for load.
	 */
	char *filename; 

} VipsForeignLoadOpenexr;

typedef VipsForeignLoadClass VipsForeignLoadOpenexrClass;

G_DEFINE_TYPE( VipsForeignLoadOpenexr, vips_foreign_load_openexr, 
	VIPS_TYPE_FOREIGN_LOAD );

static VipsForeignFlags
vips_foreign_load_openexr_get_flags( VipsForeignLoad *load )
{
	VipsForeignLoadOpenexr *openexr = (VipsForeignLoadOpenexr *) load;
	VipsForeignFlags flags;

	flags = 0;
	if( vips__openexr_istiled( openexr->filename ) )
		flags |= VIPS_FOREIGN_PARTIAL;

	return( flags );
}

static int
vips_foreign_load_openexr_header( VipsForeignLoad *load )
{
	VipsForeignLoadOpenexr *openexr = (VipsForeignLoadOpenexr *) load;

	if( vips__openexr_read_header( openexr->filename, load->out ) ) 
		return( -1 );

	return( 0 );
}

static int
vips_foreign_load_openexr_load( VipsForeignLoad *load )
{
	VipsForeignLoadOpenexr *openexr = (VipsForeignLoadOpenexr *) load;

	if( vips__openexr_read( openexr->filename, load->real ) ) 
		return( -1 );

	return( 0 );
}

static const char *vips_foreign_openexr_suffs[] = { ".exr", NULL };

static void
vips_foreign_load_openexr_class_init( VipsForeignLoadOpenexrClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	VipsObjectClass *object_class = (VipsObjectClass *) class;
	VipsForeignClass *foreign_class = (VipsForeignClass *) class;
	VipsForeignLoadClass *load_class = (VipsForeignLoadClass *) class;

	gobject_class->set_property = vips_object_set_property;
	gobject_class->get_property = vips_object_get_property;

	object_class->nickname = "openexrload";
	object_class->description = _( "load an OpenEXR image" );

	foreign_class->suffs = vips_foreign_openexr_suffs;

	load_class->is_a = vips__openexr_isexr;
	load_class->get_flags = vips_foreign_load_openexr_get_flags;
	load_class->header = vips_foreign_load_openexr_header;
	load_class->load = vips_foreign_load_openexr_load;

	VIPS_ARG_STRING( class, "filename", 1, 
		_( "Filename" ),
		_( "Filename to load from" ),
		VIPS_ARGUMENT_REQUIRED_INPUT, 
		G_STRUCT_OFFSET( VipsForeignLoadOpenexr, filename ),
		NULL );
}

static void
vips_foreign_load_openexr_init( VipsForeignLoadOpenexr *openexr )
{
}

/**
 * vips_openexrload:
 * @filename: file to load
 * @out: decompressed image
 * @...: %NULL-terminated list of optional named arguments
 *
 * Read a OpenEXR file into a VIPS image. 
 *
 * The reader can handle scanline and tiled OpenEXR images. It can't handle
 * OpenEXR colour management, image attributes, many pixel formats, anything
 * other than RGBA.
 *
 * This reader uses the rather limited OpenEXR C API. It should really be
 * redone in C++.
 *
 * See also: vips_image_new_from_file().
 *
 * Returns: 0 on success, -1 on error.
 */
int
vips_openexrload( const char *filename, VipsImage **out, ... )
{
	va_list ap;
	int result;

	va_start( ap, out );
	result = vips_call_split( "openexrload", ap, filename, out );
	va_end( ap );

	return( result );
}
