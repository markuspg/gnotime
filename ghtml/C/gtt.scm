;
; FILE:
; gtt.scm
;
; FUNCTION:
; Miscellaneous definitions for generating reports
;
; HISTORY:
; Copyright (c) 2002 Linas Vepstas <linas@linas.org>
;
; This file is covered by the GPL.  Please refer to the
; GPL license for details.


; Various bits of syntactic sugar for showing the current (linked)
; project title and other stuff of that sort.
;
(define (gtt-show-project-title) 
        (gtt-show (gtt-project-title (gtt-linked-project))))

(define (gtt-show-project-desc) 
        (gtt-show (gtt-project-desc (gtt-linked-project))))

(define (gtt-show-project-notes) 
        (gtt-show (gtt-project-notes (gtt-linked-project))))

(define (gtt-show-basic-journal)
        (gtt-show-journal (gtt-linked-project)))


;; ---------------------------------------------------------     
; Define primitives as per generic scheme
; surely these are defined in somewhere else (slib ??)
(define (xtagged-list? exp tag)
  (if (pair? exp)
      (eq? (car exp) tag)
      #f))
(define (xquoted? exp)
  (xtagged-list? exp 'quote))


;; ---------------------------------------------------------     
; The gtt-apply-func-list-to-obj routine is a simple utility 
; to apply a list of functions to a single gtt object,
; where a gtt object is a task, interval, or project.
; The 'function list' should be either:
;  -- a function that takes a single gtt object as an argument
;  -- a double-quoted string
; It returns a list of the result of applying each function
; to the object, omitting null results from the list
; 
; XXX FIXME the vars first_func, next_func, parent_obj,
; next_obj, result, appres, is-bill, is-hold, is-paid  
; are scoped globally to the functions being applied.   This 
; introducees potential symbol conflist.  This needds to be fixed!
;
(define (gtt-apply-func-list-to-obj func_list obj)
   (let ( (first_func (car func_list))
          (next_func  (cdr func_list)) 
        )
   (let (
        ; result is the result of the evaluation. 
        ; We compute it here to make sure we apply only once;
        ; we can use the result for tests.
        (result (if (xquoted? first_func) 
                      (cdr first_func) (first_func obj)))
        )
        (if (null? next_func)
           result
           ; if result was null, do not put it into list! 
           (if (null? result)
              (gtt-apply-func-list-to-obj next_func obj)
              (list 
                result
                (gtt-apply-func-list-to-obj next_func obj)))
        )
)))
           
; The gtt-apply-func-list-to-obj-list routine is a simple 
; utility to apply a list of functions to a list of gtt objects.
; The 'function list' should be either:
;  -- a function that takes a single object as an argument
;  -- a double-quoted string
; It returns a list of the result of applying each function
; to the object, omitting null results from the list
; 
(define (gtt-apply-func-list-to-obj-list func_list obj_list) 
   (if (null? obj_list) ()
   (let ( (parent_obj (car obj_list))
          (next_obj   (cdr obj_list))
        )
   (let (
        ; appres is the result of the evaluation. 
        ; We compute it here to make sure we apply only once;
        ; we can use the result for tests.
        (appres (if (list? parent_obj)
                    (gtt-apply-func-list-to-obj-list func_list parent_obj)
                    (gtt-apply-func-list-to-obj func_list parent_obj)) 
                )
        )
        (if (null? next_obj)
            appres
            (if (null? appres)
                (gtt-apply-func-list-to-obj-list func_list next_obj)
                (list appres
                     (gtt-apply-func-list-to-obj-list func_list next_obj))
            )
         )
))))

;; ---------------------------------------------------------     
; The gtt-show-projects is syntatic sugar for displaying a 
; project info with embedded html markup
;
(define (gtt-show-projects prj_list func_list) 
        (gtt-show (gtt-apply-func-list-to-obj-list func_list prj_list))
)
  
; The gtt-show-tasks proceedure is syntatic sugar for displaying a 
; task info with embedded html markup
;
(define (gtt-show-tasks task_list func_list) 
        (gtt-show (gtt-apply-func-list-to-obj-list func_list task_list))
)
  
; Syntactic sugar for organizing intervals.
(define (gtt-ivls task func_list)
        (gtt-apply-func-list-to-obj-list func_list (gtt-intervals task))
)

; Utility to compute elapsed time
(define (gtt-interval-elapsed interval)
        (- (gtt-interval-stop interval) (gtt-interval-start interval))
)
  
;; ---------------------------------------------------------     
; The gtt-task-billable-value-str routine will display the value of
; a task, but only if its been marked as 'billable'.
; XXX Need to i18n/l10n the text '"Billable", otherwise the function
; breaks

(define (gtt-task-billable-value-str task)
        (if (equal? (gtt-task-billable task) '"Billable")
            (gtt-task-value-str task) 
            '"$0.00")
)

;; ---------------------------------------------------------     
;; Define some filters to prune down task lists.
;;
;; The gtt-billable-tasks takes a list of tasks, and returns
;; a list of only those that are billable.

(define (gtt-filter-bill-tasks tasks)
        (define (is-bill task)
                (if (equal? (gtt-task-billstatus task) '"Bill")
                  task  ()))
        (gtt-apply-func-list-to-obj-list (list is-bill) tasks)
)

(define (gtt-filter-paid-tasks tasks)
        (define (is-paid task)
                (if (equal? (gtt-task-billstatus task) '"Paid")
                  task  ())
        )
        (gtt-apply-func-list-to-obj-list (list is-paid) tasks)
)

(define (gtt-filter-hold-tasks tasks)
        (define (is-hold task)
                (if (equal? (gtt-task-billstatus task) '"Hold")
                  task  ())
        )
        (gtt-apply-func-list-to-obj-list (list is-hold) tasks)
)

;; ---------------------------------------------------------     
;; --------------------- end of file -----------------------     