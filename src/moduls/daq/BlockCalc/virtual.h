
//OpenSCADA system module DAQ.BlockCalc file: virtual.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <string>
#include <vector>

#include <ttipdaq.h>
#include <tparamcontr.h>

#include "block.h"

using std::string;
using std::vector;

namespace Virtual
{

//class TConfig;
class TipContr;

class Contr: public TController
{
    friend class Block;    

    public:
	Contr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~Contr();   

	void load( );
	void save( );
	void start( );
	void stop( );
	void enable_( );
	void disable_( );	    
    
	TParamContr *ParamAttach( const string &name, int type );
	int period()  { return m_per; }
	int iterate() { return m_iter; }
	
	//Scheme's functions
        void blkList( vector<string> &ls )	{ chldList(m_bl,ls); }
        bool blkPresent( const string &id )    	{ return chldPresent(m_bl,id); }
        void blkAdd( const string &id );
        void blkDel( const string &id )    	{ chldDel(m_bl,id); }
        AutoHD<Block> blkAt( const string &id )	{ return chldAt(m_bl,id); }
	void copyBlock( const string &from_id, const string &cntr_id, const string &to_id, const string &to_name );	
	
	int res()		{ return hd_res; }
    
	TipContr &owner()	{ return (TipContr&)TController::owner(); }
    
    protected:
	bool cfgChange( TCfg &cfg );
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void loadV( );
	void saveV( );
	void freeV( );
	
        //Process stat
        void blkProc( const string & id, bool val );
	
	void postDisable(int flag);
    
    private:
	static void Task(union sigval obj);
	static void TaskDBSync(union sigval obj);
	
    private:
	bool	prc_st,      	// Calc status
		sync_st;	// Sync DB status
	int	&m_per,  	// Clock period (ms)
		&m_iter,    	// Iteration into clock
		&m_dbper;	// DB period sync (s)
	
	timer_t	sncDBTm,	// Sync DB timer
		tmId;   	// Thread timer	
	
	int	m_bl;
	vector< AutoHD<Block> >	clc_blks;	// Calc blocks HD
	double	tm_calc;			// Scheme's calc time
	
	int	hd_res;		//Resource for process block
};

class Prm : public TParamContr
{
    public:
     	Prm( string name, TTipParam *tp_prm );
    
	void enable();
	void disable();
	
	Contr &owner()  { return (Contr&)TParamContr::owner(); }
	
    private:
	void postEnable();
	void preDisable(int flag);
	
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
    
	void vlSet( TVal &val );
	void vlGet( TVal &val );
    
    private:
	string m_blck;	//Assign block
	
	TElem  v_el;	//Values elem
};

class TipContr: public TTipDAQ
{
    public:
	TipContr( string name );
	~TipContr();
	
	void modLoad( );
	
	TController *ContrAttach( const string &name, const string &daq_db );
	
	TElem &blockE()		{ return(blk_el); }
	TElem &blockIOE()	{ return(blkio_el); }

	AutoHD<Contr> at( const string &name, const string &who = "" )
	{ return TTipDAQ::at(name,who); }
	
    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
    
    private:
	void postEnable( );
	void preDisable(int flag);
        string optDescr( );
	void loadBD();
	void saveBD();
	
    private:
	TElem	blk_el;
	TElem   blkio_el;	
};

extern TipContr *mod;

} //End namespace virtual

#endif //VIRTUAL_H
