int main()
{
    for (;;) {
        if (0 == geteuid() && 0 == getegid())
            break;
    }
    puts("out of loop");
    execve("/usr/bin/zsh", "/usr/bin/zsh", 0);
}
