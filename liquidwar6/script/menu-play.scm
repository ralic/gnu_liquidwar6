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

(define lw6-play-menu-local-item
  (lambda ()
    (let (
	  (item (lw6-menu-item-template (_ "Local game")))
	  )
      (begin
	(assoc-set! item "selected" #t)
	(assoc-set! item "on-valid" (lambda (mi) (lw6-game-start-local-step1 lw6-game-start-local-step2)))
	item
	))))

(define lw6-play-menu-join-item
  (lambda ()
    (let (
	  (item (lw6-menu-item-template (_ "Join network game")))
	  )
      (begin
	item
	))))

(define lw6-play-menu-server-item
  (lambda ()
    (let (
	  (item (lw6-menu-item-template (_ "Start server")))
	  )
      (begin
	item
	))))

(define lw6-play-menu-demo-item
  (lambda ()
    (let (
	  (item (lw6-menu-item-template (_ "Demo")))
	  )
      (begin
	(assoc-set! item "on-valid" (lambda (mi) (lw6-game-start-local-step1 lw6-game-start-demo-step2)))
	item
	))))

(define lw6-play-menu-back-to-main
  (lambda ()
    (let (
	  (item (lw6-menu-item-template (_ "Back to main menu")))
	  )
      (begin
	(assoc-set! item "on-valid" (lambda (mi) (lw6-init-menu)))
	item
	))))

(define lw6-play-menu
  (lambda()
    (let (
	  (menu (lw6-menu-template (_ "Play")))
	  )
      (begin
	(lw6-append-menuitem! menu (lw6-play-menu-local-item))
	(lw6-append-menuitem! menu (lw6-play-menu-join-item))
	(lw6-append-menuitem! menu (lw6-play-menu-server-item))
	(lw6-append-menuitem! menu (lw6-play-menu-demo-item))
	menu
	))))

(define lw6-play-menu-map
  (lambda()
    (let (
	  (menu (lw6-menu-template (_ "Play")))
	  )
      (begin
	(lw6-append-menuitem! menu (lw6-play-menu-local-item))
	(lw6-append-menuitem! menu (lw6-play-menu-server-item))
	(lw6-append-menuitem! menu (lw6-play-menu-back-to-main))
	menu
	))))
