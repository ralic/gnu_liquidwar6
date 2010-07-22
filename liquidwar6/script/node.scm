;Liquid War 6 is a unique multiplayer wargame.
;Copyright (C)  2005, 2006, 2007, 2008, 2009  Christian Mauduit <ufoot@ufoot.org>
;
;This program is free software; you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;
;Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
;Contact author        : ufoot@ufoot.org

(define lw6-node-start
  (lambda ()
    (let* (
	   (cli-backends (lw6-config-get-string lw6def-cli-backends))
	   (srv-backends (lw6-config-get-string lw6def-srv-backends))
	   (bind-ip (lw6-config-get-string lw6def-bind-ip))
	   (bind-port (lw6-config-get-number lw6def-bind-port))
	   (node-id (lw6-get-game-global "node-id"))
	   (public-url (lw6-config-get-string lw6def-public-url))
	   (password (lw6-config-get-string lw6def-password))
	   (db (c-lw6p2p-db-new (c-lw6p2p-db-default-name)))
	   (node (c-lw6p2p-node-new db cli-backends srv-backends bind-ip bind-port node-id public-url password))
	   )
      (if db
	  (lw6-set-game-global! "db" db))
      (if node
	  (lw6-set-game-global! "node" node))
      )))

(define lw6-node-poll
  (lambda ()
    (let (
	  (node (lw6-get-game-global "node"))
	  )
      (if node
	  (begin
	    (c-lw6p2p-node-poll node)
	    ;;(lw6-log-notice node)
	    ))
      )))

(define lw6-node-stop
  (lambda ()
    (let (
	  (node (lw6-get-game-global "node"))
	  )
      (if node
	  (begin
	    (c-lw6p2p-node-close node)
	    ;;(lw6-log-notice node)
	    ))
      )))