
//OpenSCADA system file: tmodule.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <libintl.h>

#include "tsys.h"
#include "terror.h"
#include "tmess.h"
#include "tsubsys.h"
#include "tmodule.h"

const char *TModule::l_info[] = 
    {"Modul","Name","Type","Source","Version","Author","Descript","License"};

TModule::TModule( )
{

}

TModule::~TModule(  )
{
    //Clean export function list
    for(int i=0; i < m_efunc.size(); i++)
        delete m_efunc[i];
}

string TModule::modName()	
{ 
    return I18Ns(mName); 
}

void TModule::postEnable()
{
#if OSC_DEBUG 
    Mess->put(nodePath().c_str(),TMess::Info,Mess->I18N("Connect module!"));
#endif    

    lc_id = string("oscd_")+mId;
    bindtextdomain(lc_id.c_str(),LOCALEDIR);
}

void TModule::modFuncList( vector<string> &list )
{
    list.clear();    
    for(int i=0; i < m_efunc.size(); i++)
        list.push_back(m_efunc[i]->prot);
}

bool TModule::modFuncPresent( const string &prot )
{
    for(int i=0; i < m_efunc.size(); i++)
	if( m_efunc[i]->prot == prot )
	    return true;
    return false;	    
}

TModule::ExpFunc &TModule::modFunc( const string &prot )
{
    for(int i=0; i < m_efunc.size(); i++)
	if( m_efunc[i]->prot == prot ) return *m_efunc[i];
    throw TError(nodePath().c_str(),"Function <%s> no present into module!",prot.c_str());        
}	

void TModule::modFunc( const string &prot, void (TModule::**offptr)() )
{
    *offptr = modFunc(prot).ptr;
}

void TModule::modInfo( vector<string> &list )
{
    for( int i_opt = 0; i_opt < sizeof(l_info)/sizeof(char *); i_opt++ )
    	list.push_back( l_info[i_opt] );
}

string TModule::modInfo( const string &name )
{
    string info;
    
    if( name == l_info[0] )      info=mId;
    else if( name == l_info[1] ) info=I18Ns(mName);
    else if( name == l_info[2] ) info=I18Ns(mType);
    else if( name == l_info[3] ) info=mSource;
    else if( name == l_info[4] ) info=I18Ns(mVers);
    else if( name == l_info[5] ) info=I18Ns(mAutor);
    else if( name == l_info[6] ) info=I18Ns(mDescr);
    else if( name == l_info[7] ) info=I18Ns(mLicense);
    
    return(info);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TModule::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    	
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18N("Module: ")+modId());
	if(TUIS::presentIco(owner().subId()+"."+modId()))
	    ctrMkNode("img",opt,-1,a_path.c_str(),"/ico","",0444);
	ctrMkNode("area",opt,-1,a_path.c_str(),"/help",Mess->I18N("Help"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/help/m_inf",Mess->I18N("Module information"));
    	
	modInfo(list);
	for( int i_l = 0; i_l < list.size(); i_l++)
	    ctrMkNode("fld",opt,-1,a_path.c_str(),(string("/help/m_inf/")+list[i_l]).c_str(),I18Ns(list[i_l]),0444,0,0,1,"tp","str");	
    }
    else if( cmd==TCntrNode::Get )
    {
	if(a_path == "/ico")
	{
	    string itp;
	    opt->text(TSYS::strCode(TUIS::getIco(owner().subId()+"."+modId(),&itp),TSYS::base64));
	    opt->attr("type",itp);
	}
	else if( a_path.substr(0,11) == "/help/m_inf" )	ctrSetS( opt, modInfo(TSYS::pathLev(a_path,2)) ); 
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
	TCntrNode::cntrCmd_(a_path,opt,cmd);
}

//================== Translate functions ======================
const char *TModule::I18N( const char *mess )   
{ 
    const char *rez = Mess->I18N(mess,lc_id.c_str());
    if( !strcmp(mess,rez) ) rez = Mess->I18N(mess);
    return( rez ); 
}

string TModule::I18Ns( const string &mess ) 
{ 
    return I18N(mess.c_str());
}


