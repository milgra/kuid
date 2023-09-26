# creat fifo
mkfifo -p foo.fifo

# lock a writer process on fifo to avoid eof signals while running
exec 9<> foo.fifo

while read -r line; do
    if [ "$line" = 'quit' ]; then
        printf "%s: 'quit' command received\n"
        break
    fi
    printf "%s\n" "$line"  # print line read
done < foo.fifo
