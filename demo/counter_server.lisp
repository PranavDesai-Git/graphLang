; Helper to enforce sequential evaluation
(define seq (a b) b)

; Send the current connection count to the client and close
(define handle_client (client_sock count)
    (seq 
        (net.send client_sock (str.make count))
        (net.close client_sock)))

; Infinite loop to accept incoming connections and increment count
(define accept_loop (server_sock count)
    (seq
        (handle_client (net.accept server_sock) count)
        (accept_loop server_sock (+ count 1))))

; Start the server on port 8081
(define main ()
    (accept_loop (net.listen 8081) 1))

(main)
