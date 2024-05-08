
struct CoreModule {
private:
    int fd_module_output = -1;
    int fd_module_input  = -1;
    int fd_logger        = -1;

    const char* path_module_output;
    const char* path_module_input;
    const char* path_logger;

    void start ();
    void handshake ();
public:
    CoreModule (const char* path_module_output, const char* path_module_input, const char* path_logger);

    bool ready ();
    void close ();

    int read_logger      (char* buffer, int size);
    int read_from_module (char* buffer, int size);
    int write_to_module  (const char* buffer, int size);
};
