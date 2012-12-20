;; Liquid War 6 is a unique multiplayer wargame.
;; Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012  Christian Mauduit <ufoot@ufoot.org>
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;;
;;
;; Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
;; Contact author	 : ufoot@ufoot.org

(load "load.scm") ; load this to catch error in make dist
(lw6-load) ; will load all includes but main, trap syntax errors at least

(define lw6-test-node-b-join
  (lambda ()
    (let (
	  (ret #f)
	  (db-name (format #f "~a.test.node-b" (c-lw6p2p-db-default-name)))
	  )
      (begin
	(c-lw6net-init #t)
	;; It's important to reset test DBs before using them,
	;; else there could be temporaries IDs mismatches.
	;; In real use cases, only our local ID changes,
	;; and we really do want something with same URL but
	;; different ID than before to somewhat "not connect very well",
	;; at least for some time.
	(c-lw6p2p-db-reset db-name)
	;; Wait for a little time before trying to do anything, just to
	;; let the server fire itself, start doing things, else because
	;; of threading issue we could try to connect to a server which
	;; is not ready. Then it would finally work, but it just takes
	;; time to get back in the polling queue and have that server
	;; queried again since it was reported non-existent at first.
	(c-lw6sys-delay lw6-test-network-shift-delay)
	(let* (
	       (timestamp-0 (c-lw6sys-get-timestamp))
	       (seq-0 (c-lw6pil-suite-get-seq-0))
	       (id (c-lw6pil-suite-get-node-id 1))
	       (db (c-lw6p2p-db-new db-name))
	       (node (c-lw6p2p-node-new db (list (cons "client-backends" "tcp,udp")
						   (cons "server-backends" "tcpd,udpd,httpd")
						   (cons "bind-ip" "0.0.0.0")
						   (cons "bind-port" 8058)
						   (cons "node-id" id)
						   (cons "public-url" "http://localhost:8058/")
						   (cons "password" "")
						   (cons "title" "")
						   (cons "description" (_ "Dummy test node B"))
						   (cons "bench" 10)
						   (cons "open-relay" #f)
						   (cons "known-nodes" "http://localhost:8057/")
						   (cons "network-reliability" 100)
						   (cons "trojan" #f)
						   )))
	       (dump #f)
	       (level #f)
	       (game-struct #f)
	       (game-state #f)
	       (pilot #f)
	       (time-limit (+ lw6-test-network-global-delay (c-lw6sys-get-timestamp)))
	       (connect-time (- time-limit lw6-test-network-connect-delay))
	       (connect-ret #f)
	       (server-entry #f)
	       (connect-round (assoc-ref (c-lw6pil-suite-get-checkpoint 0) "round"))
	       )
	  (begin
	    (lw6-log-notice node)
	    ;; First, we try to establish a link to the server (OOB only)
	    (while (and (< (c-lw6sys-get-timestamp) connect-time) 
			(not server-entry))
		   (let (
			 (entries (c-lw6p2p-node-get-entries node))
			 )
		     (begin		     
		       (map (lambda(x) (if (and (equal? (assoc-ref x "url") "http://localhost:8057/")
						(assoc-ref x "id"))
					   (begin
					     (lw6-log-notice (format #f "discovered \"~a\"" x))
					     (c-lw6sys-idle)
					     (c-lw6p2p-node-poll node)
					     (set! server-entry x)					     
					     )))
			    entries)	 
		       (c-lw6sys-idle)
		       (c-lw6p2p-node-poll node)
		       )))
	    ;; Then, we wait until the server (node-a) is up-to-date enough
	    (if server-entry
		(while (and (< (c-lw6sys-get-timestamp) connect-time) 
			    (< (assoc-ref server-entry "round") connect-round))		       
		       (let (
			     (entries (c-lw6p2p-node-get-entries node))
			     )
			 (begin		     
			   (map (lambda(x) (if (and (equal? (assoc-ref x "url") "http://localhost:8057/")
						    (assoc-ref x "id"))
					       (begin
						 (c-lw6sys-idle)
						 (c-lw6p2p-node-poll node)
						 (set! server-entry x)
						 )))
				entries)	 
			   (c-lw6sys-idle)
			   (c-lw6p2p-node-poll node)
			   ))))
	    (if (and server-entry (>= (assoc-ref server-entry "round") connect-round))
		(if (c-lw6p2p-node-client-join 
		     node
		     (assoc-ref server-entry "id")
		     (assoc-ref server-entry "url"))
		    (begin
			   (map (lambda (command) (begin
						    (lw6-log-notice (format #f "sending command \"~a\" from test suite stage 2" command))
						    (c-lw6p2p-node-put-local-msg node command)
						    ))
				(c-lw6pil-suite-get-commands-by-node-index 1 1)
				)
		    (while (and (< (c-lw6sys-get-timestamp) connect-time)
				(not pilot))
			   (begin
			     (c-lw6sys-idle)
			     (c-lw6p2p-node-poll node)
			     ;; pump all draft messages
			     (let* (
				    (msg (c-lw6p2p-node-get-next-draft-msg node))
				    )
			       (while msg
				      (begin
					(lw6-test-log-message "draft" msg)
					;;(c-lw6pil-send-command pilot msg #f)
					(set! msg (c-lw6p2p-node-get-next-draft-msg node))
					)
				      ))
			     ;; pump all reference messages
			     (let* (
				    (msg (c-lw6p2p-node-get-next-reference-msg node))
				    )
			       (while msg
				      (begin
					(lw6-test-log-message "reference" msg)
					;;(c-lw6pil-send-command pilot msg #t)
					(set! msg (c-lw6p2p-node-get-next-reference-msg node))
					)
				      ))
			     )))))
	    (c-lw6p2p-node-close node)
	    ))
	(c-lw6net-quit)
	(gc)     
	ret))))

(c-lw6-set-ret #f) ;; reset this to "failed" so that it has the right value is script is interrupted
(c-lw6-set-ret (and
		(lw6-test-run lw6-test-node-b-join)
		))