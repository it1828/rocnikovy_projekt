//
//  CMBMenuNode.cpp
//  Menu_test
//
//  Created by Michael on 13.09.20.
//  Copyright © 2020 ___MICHAEL_BERNHARD___. All rights reserved.
//

#include "CMBMenuNode.hpp"

//==========================================================================================
//  default constructor
//==========================================================================================
CMBMenuNode::CMBMenuNode()
{
  init("",0);
}


//==========================================================================================
//  additional constructor
//==========================================================================================
CMBMenuNode::CMBMenuNode(const char * const f_Text,
             int f_FID,
             uint8_t f_Layer // = 0
)
{
  init(f_Text, f_FID, f_Layer);
}


//==========================================================================================
//  destructor
//==========================================================================================
CMBMenuNode::~CMBMenuNode()
{
  
}


//==========================================================================================
//  get info
//==========================================================================================
void CMBMenuNode::getInfo(const char*& f_Info)
{
  f_Info = m_Text;
}


//==========================================================================================
//  init method
//==========================================================================================
void CMBMenuNode::init(const char * const f_Text,
                 int f_FID,
                 uint8_t f_Layer // = 0
)
{
  m_Text = f_Text;
  m_FID = f_FID;
  m_Layer = f_Layer;
}


//==========================================================================================
//  set layer
//==========================================================================================
void CMBMenuNode::setLayer(uint8_t f_Layer)
{
  m_Layer = f_Layer;
}
