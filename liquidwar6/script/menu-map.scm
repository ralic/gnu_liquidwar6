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

(define lw6-map-menu-item
  (lambda (entry)
    (let* (
	   (title (assoc-ref entry "title"))
	   (menuitem (lw6-menu-item-template title))
	   (relative-path (assoc-ref entry "relative-path"))
	  )
      (if (assoc-ref entry "has-subdirs")
	  (begin
	    (set! menuitem (assoc-set! menuitem "selected" (string-prefix? relative-path (lw6-config-get-string lw6def-chosen-map))))
	    (set! menuitem (assoc-set! menuitem "label" (string-concatenate (list title "/"))))
	    (set! menuitem (assoc-set! menuitem "on-select" (lambda (mi) (begin (lw6-game-idle)))))
	    (set! menuitem (assoc-set! menuitem "on-valid" (lambda (mi) (lw6-push-menu (lw6-map-menu-relative (c-lw6sys-path-concat relative-path title))))))
	    menuitem
	    )
	  (begin
	    (set! menuitem (assoc-set! menuitem "selected" (equal? relative-path (lw6-config-get-string lw6def-chosen-map))))
	    (set! menuitem (assoc-set! menuitem "on-select" (lambda (mi) (begin (lw6-game-preview) (lw6-loader-push-if-needed relative-path) (lw6-config-set-string! lw6def-chosen-map relative-path)))))
	    (set! menuitem (assoc-set! menuitem "on-valid" (lambda (mi) (lw6-push-menu (lw6-play-menu-map)))))
	    menuitem
	    )
	  ))))

(define lw6-map-menu-item-appender
  (lambda (menu)
    (lambda (entry)
      (lw6-append-menuitem! menu (lw6-map-menu-item entry)))))

(define lw6-map-menu-relative
  (lambda (relative-path)
    (let* (
	   (menu (lw6-menu-template (_ "Choose map")))
	   (map-path (c-lw6cfg-unified-get-map-path))
	   (map-parent-path (c-lw6sys-path-parent relative-path))
	   (entries (c-lw6ldr-get-entries map-path map-parent-path))
	  )
      (begin
	(map (lw6-map-menu-item-appender menu) entries)
	(set! menu (assoc-set! menu "on-push" (lambda (m) (lw6-game-preview))))
	(set! menu (assoc-set! menu "on-pop" (lambda (m) (lw6-game-idle))))
	menu
	))))

(define lw6-map-menu
  (lambda ()
    (lw6-map-menu-relative (lw6-config-get-string lw6def-chosen-map))))

(define lw6-push-map-menu
  (lambda ()
    (let* (
	   (chosen-map (lw6-config-get-string lw6def-chosen-map))
	   (chosen-map-splitted (c-lw6sys-path-split chosen-map))
	   (relative-path "")
	  )
      (begin
	(map (lambda (path-elem) (begin 
				   (set! relative-path (c-lw6sys-path-concat relative-path path-elem))
				   (lw6-push-menu-nowarp (lw6-map-menu-relative relative-path))
				   ))
	     chosen-map-splitted)
	;(lw6-menu-warp-mouse)
	)
      )))
