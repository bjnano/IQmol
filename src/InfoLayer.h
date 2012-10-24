#ifndef IQMOL_INFOLAYER_H
#define IQMOL_INFOLAYER_H
/*******************************************************************************
       
  Copyright (C) 2011 Andrew Gilbert
           
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

#include "AtomLayer.h"
#include "DataLayer.h"
#include "DipoleLayer.h"
#include "InfoConfigurator.h"
#include <QMap>
#include <QRect>


namespace IQmol {
namespace Layer {

   // Container class for information about the whole molecule such as
   // number of atoms, charge, multiplicity etc.
   class Info : public Data {

      Q_OBJECT

      friend class Configurator::Info;

      public:
         enum EnergyUnit { Hartree, KJmol };
         explicit Info(Molecule* molecule = 0);

         void setMolecule(Molecule*);
         void addAtom(Atom const*);
         void addAtoms(AtomList const&);
         void removeAtom(Atom const*);
         void removeAtoms(AtomList const&);
         void clear();

         unsigned int numberOfElectrons() const;
         unsigned int numberOfAlphaElectrons() const;
         unsigned int numberOfBetaElectrons() const;
         QString formula() const;
         int  getCharge() const { return m_charge; }
         unsigned int  getMultiplicity() const { return m_multiplicity; }
         void dump();

      public Q_SLOTS:
         void setCharge(int const charge);
         void setMultiplicity(unsigned int const multiplicity);
         void setEnergy(double const energy, Info::EnergyUnit unit);
         void setDipole(qglviewer::Vec const& dipole, bool const estimated = false);
         void setPointGroup(QString const& pointGroup);

      Q_SIGNALS:
         void updated(); 

      protected:
         double dipole() const { return m_dipole.value(); }
         int m_charge;
         double m_energy;
         double m_mass;
         unsigned int m_numberOfAtoms;
         unsigned int m_nuclearCharge;
         unsigned int m_multiplicity;
         EnergyUnit m_energyUnit;
         QMap<QString, int> m_formula;
         QString m_pointGroup;
         bool m_dipoleEstimated;
         bool m_suspendUpdate;

      private:
         void setDipoleValid(bool);
         Configurator::Info m_configurator;
         Dipole m_dipole;
   };


} } // end namespace IQmol::Layer

#endif