/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#pragma once

#include "gui_wallet_tabcontentmanager.hpp"

namespace gui_wallet
{
   class DecentTable;
   class DecentLineEdit;
   struct SDigitalContent;

   class BrowseContentTab : public TabContentManager
   {
      Q_OBJECT
   public:
      BrowseContentTab(QWidget* pParent,
                       DecentLineEdit* pFilterLineEdit);
      
      void ShowDigitalContentsGUI();
      
   public:
      virtual void timeToUpdate(const std::string& result) override;
      virtual std::string getUpdateCommand() override;
       
   public slots:

      void slot_Details(int);
      void slot_Bought();
      void slot_SearchTermChanged(QString const& strSearchTerm);
      void slot_SortingChanged(int);
      void slot_cellClicked(int row, int col);
      
   protected:
      DecentTable*   m_pTableWidget;
      QString        m_strSearchTerm;
      
      std::vector<SDigitalContent>  _digital_contents;
   };
}
