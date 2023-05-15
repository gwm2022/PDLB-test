/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */

/*
 * Copyright (C) 1997 by the University of Southern California
 * $Id: classifier-mpath.cc,v 1.10 2005/08/25 18:58:01 johnh Exp $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * The copyright of this module includes the following
 * linking-with-specific-other-licenses addition:
 *
 * In addition, as a special exception, the copyright holders of
 * this module give you permission to combine (via static or
 * dynamic linking) this module with free software programs or
 * libraries that are released under the GNU LGPL and with code
 * included in the standard release of ns-2 under the Apache 2.0
 * license or under otherwise-compatible licenses with advertising
 * requirements (or modified versions of such code, with unchanged
 * license).  You may copy and distribute such a system following the
 * terms of the GNU GPL for this module and the licenses of the
 * other code concerned, provided that you include the source code of
 * that other code when and as the GNU GPL requires distribution of
 * source code.
 *
 * Note that people who make modified versions of this module
 * are not obligated to grant this special exception for their
 * modified versions; it is their choice whether to do so.  The GNU
 * General Public License gives permission to release a modified
 * version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this
 * exception.
 *
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/classifier/classifier-mpath.cc,v 1.10 2005/08/25 18:58:01 johnh Exp $ (USC/ISI)";
#endif

#include "classifier.h"
#include<stdio.h> 
#include<sys/time.h>
#include<time.h>
#include "random.h" 
#include "/home/gao/ns-allinone-2.35/ns-2.35/common/ip.h"
#include "/home/gao/ns-allinone-2.35/ns-2.35/common/packet.h"
double start[2000],end[2000];
long ustart[2000]={0,0,0,0,0,0};
long mstart[2000]={0,0,0,0,0,0};
long uend[2000]={0,0,0,0,0,0};
long mend[2000]={0,0,0,0,0,0};
double dif_usec[2000]={0,0,0,0,0,0};
long dif_msec[2000]={0,0,0,0,0,0};
int pid[2000]={0,0,0,0,0,0};
int clflag[2000];
//int a[4]={0,0,1,1};


class MultiPathForwarder : public Classifier {
public:
	MultiPathForwarder() : ns_(0) {} 
	virtual int classify(Packet* p) {
		int cl;
		int cl1[2000];
		int fail = ns_;
		hdr_ip* iph=hdr_ip::access(p);
        int fid = iph->flowid();
		int databytes=hdr_cmn::access(p)->size();
		//double timestamp= Scheduler::instance().clock();
		//Tcl::instance().evalf("puts [format \"starttime: %lf  \"]",timestamp);
		//do {
		
		  if(pid[fid]==0){
				start[fid]=Scheduler::instance().clock();
                        Tcl::instance().evalf("puts [format \"starttime: %lf  \"]",start[fid]);
				//cl1[fid] =a[fid];
                                cl1[fid]=(int)Random::uniform(0,maxslot_+1);
			    pid[fid]++;
				clflag[fid]=cl1[fid];
				cl=cl1[fid];
				Tcl::instance().evalf("puts [format \"cl: %d  \"]",cl);
			}
			//cl = ns_++; 
			//ns_ %= (maxslot_ + 1);
			else if(databytes==1500){
				end[fid]=Scheduler::instance().clock();
				dif_usec[fid]=end[fid]-start[fid];
				 Tcl::instance().evalf("puts [format \"endtime: %lf  \"]",end[fid]);
				if (dif_usec[fid]<0.0005){
					cl1[fid]=clflag[fid];
					start[fid]=end[fid];
				    pid[fid]++;
					cl=cl1[fid];
				    }
				else{
					cl1[fid] =(int)Random::uniform(0,maxslot_+1);
					start[fid]=end[fid];
					clflag[fid]=cl1[fid];
					pid[fid]++;
					cl=cl1[fid];
				}
				Tcl::instance().evalf("puts [format \"dif_usec: %lf  %d %d %d\"]",dif_usec[fid],fid,pid[fid],cl);
			}
			//Tcl::instance().evalf("puts [format \"dif_usec: %lf  %d %d %d\"]",dif_usec[fid],cl,fid,pid[fid]);
		
		//Tcl::instance().evalf("puts [format \"dif_usec: %lf  %d %d %d\"]",dif_usec[fid],fid,pid[fid],cl);
		else{
		do{cl = ns_++; 
		ns_ %= (maxslot_ + 1);}while (slot_[cl] == 0 && ns_ != fail);
		}
		//} while (slot_[cl] == 0 && ns_ != fail);
		//Tcl::instance().evalf("puts [format \"dif_usec: %lf  %d %d %d\"]",dif_usec[fid],cl,fid,pid[fid]);
		return cl;
		
	}
private:
	int ns_;
};

static class MultiPathClass : public TclClass {
public:
	MultiPathClass() : TclClass("Classifier/MultiPath") {} 
	TclObject* create(int, const char*const*) {
		return (new MultiPathForwarder());
	}
} class_multipath;


