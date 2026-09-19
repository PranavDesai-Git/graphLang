; Helper to enforce sequential evaluation of side effects
(define seq (a b) b)

; Handles a single client connection: reads up to 1024 bytes, sends it back, and closes.
(define handle_client (client_sock)
    (seq 
        (net.send client_sock (net.recv client_sock 1024))
        (net.close client_sock)))

; Infinite loop to accept incoming connections
(define accept_loop (server_sock)
    (seq
        (handle_client (net.accept server_sock))
        (accept_loop server_sock)))

; Start the server on port 8080
(define main ()
    (accept_loop (net.listen 8080)))

(main)
