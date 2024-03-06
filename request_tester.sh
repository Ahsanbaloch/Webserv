# curl -X POST \
#   -H "Content-Type: application/json" \
#   -d '{"key1":"value1", "key2":"value2"}' \
#   http://localhost:4141

# curl -X POST http://localhost:4141 \
#      -H "Transfer-Encoding: chunked" \
#      -H "Content-Type: application/octet-stream" \
#      --data-binary "Chunk 1"\
#      --data-binary "Chunk 2"\
#      --data-binary "Chunk 3"

curl -H "Transfer-Encoding: chunked" -d "Hello World" http://localhost:4141
