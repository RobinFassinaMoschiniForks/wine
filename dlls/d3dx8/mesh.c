/*
 * Copyright 2008 David Adam
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "wine/debug.h"

#include "d3dx8_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(d3dx);

BOOL WINAPI D3DXBoxBoundProbe(CONST D3DXVECTOR3 *pmin, CONST D3DXVECTOR3 *pmax, CONST D3DXVECTOR3 *prayposition, CONST D3DXVECTOR3 *praydirection)

/* Algorithm taken from the article: An Efficient and Robust Ray-Box Intersection Algoritm
Amy Williams             University of Utah
Steve Barrus             University of Utah
R. Keith Morley          University of Utah
Peter Shirley            University of Utah

International Conference on Computer Graphics and Interactive Techniques  archive
ACM SIGGRAPH 2005 Courses
Los Angeles, California

This algorithm is free of patents or of copyrights, as confirmed by Peter Shirley himself.

Algorithm: Consider the box as the intersection of three slabs. Clip the ray
against each slab, if there's anything left of the ray after we're
done we've got an intersection of the ray with the box.
*/

{
    FLOAT div, tmin, tmax, tymin, tymax, tzmin, tzmax;

    div = 1.0f / praydirection->x;
    if ( div >= 0.0f )
    {
     tmin = ( pmin->x - prayposition->x ) * div;
     tmax = ( pmax->x - prayposition->x ) * div;
    }
    else
    {
     tmin = ( pmax->x - prayposition->x ) * div;
     tmax = ( pmin->x - prayposition->x ) * div;
    }

    if ( tmax < 0.0f ) return FALSE;

    div = 1.0f / praydirection->y;
    if ( div >= 0.0f )
    {
     tymin = ( pmin->y - prayposition->y ) * div;
     tymax = ( pmax->y - prayposition->y ) * div;
    }
    else
    {
     tymin = ( pmax->y - prayposition->y ) * div;
     tymax = ( pmin->y - prayposition->y ) * div;
    }

    if ( ( tymax < 0.0f ) || ( tmin > tymax ) || ( tymin > tmax ) ) return FALSE;

    if ( tymin > tmin ) tmin = tymin;
    if ( tymax < tmax ) tmax = tymax;

    div = 1.0f / praydirection->z;
    if ( div >= 0.0f )
    {
     tzmin = ( pmin->z - prayposition->z ) * div;
     tzmax = ( pmax->z - prayposition->z ) * div;
    }
    else
    {
     tzmin = ( pmax->z - prayposition->z ) * div;
     tzmax = ( pmin->z - prayposition->z ) * div;
    }

    if ( (tzmax < 0.0f ) || ( tmin > tzmax ) || ( tzmin > tmax ) ) return FALSE;

    return TRUE;
}

UINT WINAPI D3DXGetFVFVertexSize(DWORD FVF)
{
  FIXME("(void): stub\n");
  return 0;
}

BOOL CDECL D3DXIntersectTri(CONST D3DXVECTOR3 *p0, CONST D3DXVECTOR3 *p1, CONST D3DXVECTOR3 *p2, CONST D3DXVECTOR3 *praypos, CONST D3DXVECTOR3 *praydir, FLOAT *pu, FLOAT *pv, FLOAT *pdist)
{
    D3DXMATRIX m;
    D3DXVECTOR4 vec;

    m.m[0][0] = p1->x - p0->x;
    m.m[1][0] = p2->x - p0->x;
    m.m[2][0] = -praydir->x;
    m.m[3][0] = 0.0f;
    m.m[0][1] = p1->y - p0->z;
    m.m[1][1] = p2->y - p0->z;
    m.m[2][1] = -praydir->y;
    m.m[3][1] = 0.0f;
    m.m[0][2] = p1->z - p0->z;
    m.m[1][2] = p2->z - p0->z;
    m.m[2][2] = -praydir->z;
    m.m[3][2] = 0.0f;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;

    vec.x = praypos->x - p0->x;
    vec.y = praypos->y - p0->y;
    vec.z = praypos->z - p0->z;
    vec.w = 0.0f;

    if ( D3DXMatrixInverse(&m, NULL, &m) )
    {
        D3DXVec4Transform(&vec, &vec, &m);
        if ( (vec.x >= 0.0f) && (vec.y >= 0.0f) && (vec.x + vec.y <= 1.0f) && (vec.z >= 0.0f) )
        {
            *pu = vec.x;
            *pv = vec.y;
            *pdist = fabs( vec.z );
            return TRUE;
        }
    }

return FALSE;
}

BOOL WINAPI D3DXSphereBoundProbe(CONST D3DXVECTOR3 *pcenter, FLOAT radius, CONST D3DXVECTOR3 *prayposition, CONST D3DXVECTOR3 *praydirection)
{
    D3DXVECTOR3 difference;
    FLOAT a, b, c, d;

    a = D3DXVec3LengthSq(praydirection);
    if (!D3DXVec3Subtract(&difference, prayposition, pcenter)) return FALSE;
    b = D3DXVec3Dot(&difference, praydirection);
    c = D3DXVec3LengthSq(&difference) - radius * radius;
    d = b * b - a * c;

    if ( ( d <= 0.0f ) || ( sqrt(d) <= b ) ) return FALSE;
    return TRUE;
}
