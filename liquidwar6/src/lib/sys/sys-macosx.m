/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009  Christian Mauduit <ufoot@ufoot.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  

  Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
  Contact author        : ufoot@ufoot.org
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef LW6_MAC_OS_X

#include "sys.h"
#include "sys-internal.h"

#import <Cocoa/Cocoa.h>

void
_lw6sys_macosx_alert (char *title, char *msg)
{
  @try {
    NSString *ns_title = [NSString stringWithUTF8String:title];
    NSString *ns_msg = [NSString stringWithUTF8String:msg];
    NSAutoreleasePool* pool = NULL;
    NSApplication* app = NULL; 

    pool=[[NSAutoreleasePool alloc] init];
    @try {
      [[NSApplication alloc] init];
    }
    @catch(NSException *exception) {
      /* 
       * depends on wether we're in Cocoa app context or not,
       * if already initialize (through SDL for instance) then
       * do not try to do it again, would fail, we just ignore
       * this error.
       */
    }
    NSRunAlertPanel (ns_title, ns_msg, nil, nil, nil);
    [ns_msg release];
    [ns_title release]; // is this correct?
    [pool release];
  }
  @catch (NSException *exception) {
    NSLog(@"Caught %@: %@", [exception name], [exception reason]);
  }

  /*
   * Fires ugly "leaking" message on exit, any Mac OS X / Cocoa / Objc
   * welcome to help fix this...
   */
}
#endif
