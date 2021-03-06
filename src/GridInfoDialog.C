/*******************************************************************************
         
  Copyright (C) 2011-2013 Andrew Gilbert
      
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

#include "GridInfoDialog.h"
#include "QMsgBox.h"
#include "MoleculeLayer.h"
#include "Preferences.h"
#include <QMenu>
#include <QHeaderView>
#include <QDebug>


using namespace qglviewer;

namespace IQmol {

GridInfoDialog::GridInfoDialog(GridList* availableGrids, Layer::Molecule* molecule,
   QWidget* parent) : QDialog(parent), m_gridList(availableGrids), m_molecule(molecule)
{
   m_dialog.setupUi(this);

   QTableWidget* table(m_dialog.gridTable);
   table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
   table->horizontalHeader()->setStretchLastSection(true);
   table->setColumnWidth(0,120);
   table->setColumnWidth(1,100);
   table->setColumnWidth(2,100);
   table->setContextMenuPolicy(Qt::CustomContextMenu);

   QString text("Step Size (");
   text += QChar(0x00c5);
   text += ")";
   table->horizontalHeaderItem(2)->setText(text);

   connect(table, SIGNAL(customContextMenuRequested(QPoint const&)),
      this, SLOT(contextMenu(QPoint const&)));

   loadGridInfo();
}


void GridInfoDialog::loadGridInfo()
{
   QTableWidget* table(m_dialog.gridTable);
   QTableWidgetItem* item;
   QString text;

   table->clearContents();
   table->setRowCount(m_gridList->size());
   int row(0);
   GridList::iterator iter;
   for (iter = m_gridList->begin(); iter != m_gridList->end(); ++iter, ++row) {

       // Type 
       item = new QTableWidgetItem();
       table->setItem(row, 0, item);
       item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
       text = (*iter)->dataType().info(); 
       item->setText(text);

       // Data Size 
       item = new QTableWidgetItem();
       item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
       text = QString::number((*iter)->dataSizeInKb(), 'f', 1);
       item->setText(text+"    ");
       table->setItem(row, 1, item);

       // Step Size 
       item = new QTableWidgetItem();
       item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
       text = QString::number((*iter)->stepSize(), 'f', 4);
       item->setText(text+"    ");
       table->setItem(row, 2, item);

       // Bounding box
       item = new QTableWidgetItem();
       item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
       Vec bbMin = (*iter)->size().bbMin();
       Vec bbMax = (*iter)->size().bbMax();
       QChar arrow(0x2192);
       text = "(" + QString::number(bbMin.x, 'f', 2) + ", "
                  + QString::number(bbMin.y, 'f', 2) + ", "
                  + QString::number(bbMin.z, 'f', 2) + ") " + arrow + " ("
                  + QString::number(bbMax.x, 'f', 2) + ", "
                  + QString::number(bbMax.y, 'f', 2) + ", "
                  + QString::number(bbMax.z, 'f', 2) + ") ";

       item->setText(text);
       table->setItem(row, 3, item);
   }
}


void GridInfoDialog::contextMenu(QPoint const& point)
{
   QTableWidget* table(m_dialog.gridTable);
   QMenu menu(this);

   menu.addAction("Delete", this, SLOT(deleteGrid()));
   menu.addAction("Export Cube File", this, SLOT(exportCubeFilePositive()));
   menu.addAction("Export Cube File (Switch Phase)", this, SLOT(exportCubeFileNegative()));

   menu.exec(table->mapToGlobal(point));
}


void GridInfoDialog::deleteGrid()
{
   GridList grids(getSelectedGrids());
   GridList::iterator iter;
   for (iter = grids.begin(); iter != grids.end(); ++iter) {
       m_gridList->removeAll(*iter);
       delete (*iter);
   }
   loadGridInfo();
}


void GridInfoDialog::exportCubeFile(bool const invertSign)
{
   if (!m_molecule) {
      QMsgBox::warning(this, "IQmol", "Unable to determine parent molecule");
      return;
   }

   GridList grids(getSelectedGrids());
   GridList::iterator iter;
   for (iter = grids.begin(); iter != grids.end(); ++iter) {
       QFileInfo fileInfo(Preferences::LastFileAccessed());
       QString name(m_molecule->text());
       name += "_" + (*iter)->dataType().info() + ".cube";
       name.replace(" ","_");
       fileInfo.setFile(fileInfo.dir(), name);
       name = fileInfo.filePath();
       if ((*iter)->saveToCubeFile(name, *m_molecule, invertSign)) {
          Preferences::LastFileAccessed(name);
          qDebug() << "Cube data saved to " << name;
       }else {
          QString msg("Unable to save to file ");
          msg += name + "\n";
          msg + "File may already exist.";
          QMsgBox::warning(this, "IQmol", msg);
       }
   }
}


GridList GridInfoDialog::getSelectedGrids()
{
   QTableWidget* table(m_dialog.gridTable);
   GridList grids;

   QList<QTableWidgetItem*> items(table->selectedItems());
   QList<QTableWidgetItem*>::iterator iter;

   int row, col;
   for (iter = items.begin(); iter != items.end(); ++iter) {
       row = (*iter)->row();
       col = (*iter)->column();
       if (col == 0 && row >= 0 && row < m_gridList->size()) grids.append((*m_gridList)[row]);
   }

   return grids; 
}


} // end namespace IQmol
