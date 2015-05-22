//OpenSCADA system module DAQ.EthernetIP file: module.h
/***************************************************************************
 *   Copyright (C) 2015 by Anibal Limon, <limon.anibal@gmai.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

#include <tsys.h>
#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"EthernetIP"
#define DAQ_NAME	_("EthernetIP")
#define DAQ_TYPE	SDAQ_ID
#define DAQ_SUBVER	SDAQ_VER
#define DAQ_MVER	"0.0.1"
#define DAQ_AUTHORS	_("Anibal Limon")
#define DAQ_DESC	_("DAQ's subsystem EthernetIP module.")
#define DAQ_LICENSE	"GPLv2"
//*************************************************

namespace EthernetIP
{

//*************************************************
//* EthernetIP::TMdPrm                            *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	TMdContr &owner( );

    protected:
	void load_( );
	void save_( );

    private:
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlArchMake( TVal &val );

	TElem	p_el;
};

//*************************************************
//* EthernetIP::TMdContr                          *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	int64_t	period( )	{ return mPer; }
	string  cron( )         { return mSched; }
	int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	void prmEn( const string &id, bool val );

	void start_( );
	void stop_( );

    private:
	TParamContr *ParamAttach( const string &name, int type );
	void cntrCmdProc( XMLNode *opt );
	static void *Task( void *icntr );

	Res	en_res;		// Resource for enable params
	TCfg	&mSched,	// Schedule
		&mPrior;	// Process task priority
	int64_t	mPer;

	bool	prcSt,		// Process task active
 		callSt,		// Calc now stat
		endrunReq;	// Request to stop of the Process task

	vector< AutoHD<TMdPrm> >  p_hd;

	double	tmGath;		// Gathering time
};

//*************************************************
//* EthernetIP::TTpContr                          *
//*************************************************
class TTpContr: public TTypeDAQ
{
    public:
	TTpContr( string name );
	~TTpContr( );

    protected:
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	TController *ContrAttach( const string &name, const string &daq_db );

	string optDescr( );
};

extern TTpContr *mod;

} //End namespace EthernetIP

#endif //MODULE_H