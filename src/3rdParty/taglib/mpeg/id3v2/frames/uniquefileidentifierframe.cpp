/***************************************************************************
    copyright            : (C) 2004 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 ***************************************************************************/

#include <tbytevectorlist.h>

#include "uniquefileidentifierframe.h"

using namespace TagLib;
using namespace ID3v2;

class UniqueFileIdentifierFrame::UniqueFileIdentifierFramePrivate
{
public:
  String owner;
  ByteVector identifier;
};

////////////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////////////

UniqueFileIdentifierFrame::UniqueFileIdentifierFrame(const ByteVector &data) :
    ID3v2::Frame(data)
{
  d = new UniqueFileIdentifierFramePrivate;
  setData(data);
}

UniqueFileIdentifierFrame::UniqueFileIdentifierFrame(const String &owner, const ByteVector &id) :
    ID3v2::Frame("UFID")
{
  d = new UniqueFileIdentifierFramePrivate;
  d->owner = owner;
  d->identifier = id;
}

UniqueFileIdentifierFrame::~UniqueFileIdentifierFrame()
{
  delete d;
}

String UniqueFileIdentifierFrame::owner() const
{
    return d->owner;
}

ByteVector UniqueFileIdentifierFrame::identifier() const
{
  return d->identifier;
}

void UniqueFileIdentifierFrame::setOwner(const String &s)
{
  d->owner = s;
}

void UniqueFileIdentifierFrame::setIdentifier(const ByteVector &v)
{
  d->identifier = v;
}

String UniqueFileIdentifierFrame::toString() const
{
  return String::null;
}

void UniqueFileIdentifierFrame::parseFields(const ByteVector &data)
{
  ByteVectorList fields = ByteVectorList::split(data, char(0));

  if(fields.size() != 2)
    return;

  d->owner = fields.front();
  d->identifier = fields.back();
}

ByteVector UniqueFileIdentifierFrame::renderFields() const
{
  ByteVector data;

  data.append(d->owner.data(String::Latin1));
  data.append(char(0));
  data.append(d->identifier);

  return data;
}

UniqueFileIdentifierFrame::UniqueFileIdentifierFrame(const ByteVector &data, Header *h) :
  Frame(h)
{
  d = new UniqueFileIdentifierFramePrivate;
  parseFields(fieldData(data));
}
