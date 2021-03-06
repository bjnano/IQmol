/*******************************************************************************
       
  Copyright (C) 2011-13 Andrew Gilbert
           
  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.
       
  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
      
  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.  
   
********************************************************************************/

#include "XyzParser.h"
#include "CartesianCoordinatesParser.h"
#include "TextStream.h"

#include <QtDebug>


namespace IQmol {
namespace Parser2 {

Data::Bank& Xyz::parse(TextStream& textStream)
{
   Data::GeometryList* geometryList(new Data::GeometryList);
   Data::Geometry* geometry(0);

   while (!textStream.atEnd()) {
      geometry = readNextGeometry(textStream);
      if (geometry) geometryList->append(geometry);
   }

   if (geometryList->isEmpty()) {
      QString msg("No coordinates found");
      m_errors.append(msg);
      delete geometryList;
   }else {
      m_dataBank.append(geometryList);
   }

   return m_dataBank;
}


Data::Geometry* Xyz::readNextGeometry(TextStream& textStream)
{
   // We look for a line with a single integer giving the number of atoms, this
   // may be the previousLine in the TextStream if the XYZ blocks are back to
   // back. Note that the number is maximal, i.e. if there are more coordinates
   // they will not be read.   

   Data::Geometry* geometry(0);
   QRegExp integerOnly("^\\d+$");
   QString line(textStream.previousLine());
   if (!line.contains(integerOnly)) {
      line = textStream.seek(integerOnly);
   }

   bool ok;
   int n(line.toInt(&ok));

   if (ok) {
      textStream.skipLine();   // skip comment line
      CartesianCoordinates parser(n);
      geometry = parser.parse(textStream);

      if (geometry) {
         QString error(parser.error());
         if (!error.isEmpty()) m_errors.append(error);
      }else {
         m_errors.append("No coordinates found");
      }
   }

   return geometry;
}

} } // end namespace IQmol::Parser
