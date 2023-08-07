// -----------------------------------------------------------------------------
// MODES_C
//
// - Program operation modes routines
//
// -----------------------------------------------------------------------------
// Copyright (c) 2023 Jorge Botana Mtz. de Ibarreta
//
// This source code is distributed under the terms of the MIT License. For more
// information, please see the LICENSE.txt file or refer to the following link:
//
//                                           https://opensource.org/license/mit/
// -----------------------------------------------------------------------------

#ifndef   WIN32
#error    "This program is targeted to systems running a Microsoft Windows OS!"
#else  // WIN32

// ------------------------ Private headers includes ------------------------ //

#include  <direct.h>
#include  <time.h>
#include  "public.h"

// ---------------------- Private preprocessor macros ----------------------- //

#define   MIN_USER_LEN                                                         1
#define   MAX_USER_LEN                                                        27
#define   MIN_COMP_LEN                                                         1
#define   MAX_COMP_LEN                                                        27

#define   MIN_BN_LEN                                                           1
#define   MAX_BN_LEN                                                          27
#define   MIN_SN_LEN                                                           1
#define   MAX_SN_LEN                                                          27

#define   N_TESTS_DIGS                                                         2

// -------------------- Private data types declarations --------------------- //

// --------------- Public global data holders initializations --------------- //

// -------------- Private global data holders initializations --------------- //

static const char folder[] = "reports";
static const char   temp[] = "incomplete.hwtt";

static FILE *csv = NULL;

// --------------------- Private functions declarations --------------------- //

static int init_files(int prod);
static int shut_files(int prod);
static void get_trace(char *user_dat, char *comp_dat, char *bn_dat,
        char *sn_dat);
static void add_csv(const char *user_dat, const char *comp_dat,
        const char *bn_dat, const char *sn_dat, const struct tm *time_struct,
        int all_ok);

// ---------------------- Public functions definitions ---------------------- //

void run_full(int prod) {
    // Print the initial header with the build information on the screen.
    char header[DEF_SMA_BUF_SIZE] = {0};
    if (prod == TRUE) {
        sprintf(header, "%s", "HWTT Production Report");
    } else {
        sprintf(header, "%s", "HWTT Testing Report");
    }
    write_header(stdout, NULL, header);
    show_version(stdout, NULL);

    // Create the TXT report and/or create/open the traceability CSV.
    ret = init_files(prod);
    if (ret != 0) {
        return;
    }

    // Print the initial header with the build information on the TXT report.
    write_header(NULL, report, header);
    show_version(NULL, report);

    // Initialize the communications.
    ret = init_coms();
    if (ret != 0) {
        shut_files(prod);

        return;
    }

    // Request the traceability information.
    static char user[MAX_USER_LEN + NULL_TERMIN_SIZE] = DEF_USER;
    static char comp[MAX_COMP_LEN + NULL_TERMIN_SIZE] = DEF_COMP;
    static char   bn[  MAX_BN_LEN + NULL_TERMIN_SIZE] = DEF_BN;
    static char   sn[  MAX_SN_LEN + NULL_TERMIN_SIZE] = DEF_SN;
    get_trace(user, comp, bn, sn);

    // Execute all the tests.
    for (int i = 0; i < N_TESTS; i++) {
        ret = exe_test(i);
        if (ret != 0) {
            shut_coms();
            shut_files(prod);

            return;
        }
    }

    // Print the results.
    write_header(stdout, report, "Tests Completed");
    for (int i = 0; i < N_TESTS; i++) {
        dis_res(i);
    }
    output(stdout, report, "\n");

    // Print the system's time and date.
    time_t epoch_secs = 0;
    time(&epoch_secs);
    char date_long[DEF_SMA_BUF_SIZE] = {0};
    struct tm *time_struct = localtime(&epoch_secs);
    strftime(date_long, sizeof(date_long),
            " -> Time and Date : %A, %B %d, %Y - %H:%M:%S", time_struct);
    output(stdout, report, date_long);
    output(stdout, report, "\n");

    // In the production mode, update the traceability CSV with the results of
    // the tests.
    if (prod == TRUE) {
        add_csv(user, comp, bn, sn, time_struct, all_ok);
        output(stdout, report, "\n");
        output(stdout, report, " -> Traceability CSV updated.");
        output(stdout, report, "\n");
    }

    // Print an empty line that marks the end of the TXT report.
    write_header(stdout, report, "");

    // Close all the communications.
    shut_coms();

    // Close all the opened files.
    shut_files(prod);

    // Get the new name of the TXT report with the batch number, serial number
    // and whole result of the tested PCBA.
    char file[DEF_SMA_BUF_SIZE] = {0};
           if (prod == FALSE && all_ok == FALSE) {
        sprintf(file,"_test_%s_%s_ERROR.txt", bn, sn);
    } else if (prod == FALSE && all_ok ==  TRUE) {
        sprintf(file,"_test_%s_%s_OK.txt"   , bn, sn);
    } else if (prod ==  TRUE && all_ok == FALSE) {
        sprintf(file,      "%s_%s_ERROR.txt", bn, sn);
    } else {
        sprintf(file,      "%s_%s_OK.txt"   , bn, sn);
    }

    // Set the folder to save the new TXT report.
    char path[DEF_MED_BUF_SIZE] = {0};
    sprintf(path, "%s\\%s", folder, file);

    // Insert the new TXT report in the folder, after deleting an old one with
    // the same name if exists.
    ret = remove(path);
    output(stdout, NULL, " -> ");
    if (ret != 0 && errno == EACCES) {
        char overwrite_error[DEF_MED_BUF_SIZE] = {0};
        sprintf(
                overwrite_error,
                "Old TXT report exists that could not be replaced. The current "
                "TXT report was saved as \"%s\". Before continue, delete manual"
                "ly the old one, rename the new one and move it to the \"report"
                "s\" folder.",
                temp
        );
        output(stdout, NULL, overwrite_error);
    } else {
        rename(temp, path);
        char save_msg[DEF_MED_BUF_SIZE] = {0};
        sprintf(save_msg, "Saved as %s", file);
        output(stdout, NULL, save_msg);
    }
    output(stdout, NULL, "\n");
    output(stdout, NULL, "\n");
}

void run_single(void) {
    // Print the initial header with the build information.
    const char header[] = "HWTT Single Test";
    write_header(stdout, NULL, header);
    show_version(stdout, NULL);

    // Initialize the communications.
    ret = init_coms();
    if (ret != 0) {
        return;
    }

    // Select and execute the desired tests and shutdown the communications when
    // finished (or in case of error).
    int more = 0;
    do {
        write_header(stdout, NULL, "Test Selection");
        const char test_fie[]  = " <- Select test number : ";
        char       test_dat[N_TESTS_DIGS + NULL_TERMIN_SIZE] = {0};
        size_t     test_len = strlen(test_dat);
        input(stdout, NULL, test_fie, test_dat, test_len, 1, N_TESTS_DIGS,
                numbers);
        int test_num = atoi(test_dat);
        if (test_num < N_TESTS) {
            ret = exe_test(test_num);
            if (ret != 0) {
                break;
            }
            write_header(stdout, NULL, "");
            ask_yes_no(stdout, NULL, " <- Execute more tests? [Y/N] : ", &more);
        } else {
            output(stdout, NULL, "\n");
            output(stdout, NULL, " -> Test number out of range [0, N_TESTS - 1]"
                    ".");
            output(stdout, NULL, "\n");
        }
    } while (more == YES);

    // Close all the communications.
    shut_coms();
    output(stdout, NULL, "\n");
}

// --------------------- Private functions definitions ---------------------- //

static int init_files(int prod) {
    // Print an initial header.
    write_header(stdout, NULL, "Files Setup");

    // Create and/or open the folder where the TXT reports generated after
    // performing the tests are going to be saved.
    output(stdout, NULL, " -> Opening reports folder .... ");
    ret = _mkdir(folder);
    if (ret != 0 && errno != EEXIST) {
        output(stdout, NULL, error_msg);
        output(stdout, NULL, "\n");
        print_error("The complete TXT reports folder could not be accessed.");

        return 1;
    }
    output(stdout, NULL, ok_msg);
    output(stdout, NULL, "\n");

    // Create the (temporal) TXT report file. If it already exists, it is
    // deleted and created again.
    output(stdout, NULL, " -> Opening the report file ... ");
    report = fopen(temp, "wb+");
    if (report == NULL) {
        output(stdout, NULL, error_msg);
        output(stdout, NULL, "\n");
        print_error("The temporal report file could not be accessed.");

        return 1;
    }
    setvbuf(report, NULL, _IONBF, 0);
    output(stdout, NULL, ok_msg);

    // In the production mode, open the CSV file (creating it if needed).
    if (prod == TRUE) {
        output(stdout, NULL, "\n");
        output(stdout, NULL, " -> Opening the CSV file ...... ");
        char buf[DEF_SMA_BUF_SIZE] = {0};
        sprintf(buf, "%s.csv", PCBA_VERSION);
        csv = fopen(buf, "ab+");
        if (csv == NULL) {
            output(stdout, NULL, error_msg);
            output(stdout, NULL, "\n");
            print_error("The traceability CSV file could not be accessed.");
            fclose(report);

            return 1;
        }
        setvbuf(csv, NULL, _IONBF, 0);
        output(stdout, NULL, ok_msg);
    }

    return 0;
}

static int shut_files(int prod) {
    // In the production mode, close the traceability CSV file.
    if (prod == TRUE) {
        ret = fclose(csv);
        if (ret != 0) {
            fclose(report);

            return 1;
        }
    }

    // Close the TXT report file.
    ret = fclose(report);
    if (ret != 0) {
        return 1;
    }

    return 0;
}

static void get_trace(char *user_dat, char *comp_dat, char *bn_dat,
        char *sn_dat) {
    // Print an initial header.
    write_header(stdout, report, "Traceability Information");

    // Request the identification of the user who performs the tests. It is
    // saved so that it is not requested again if more PCBAs are tested during
    // the execution of the program.
    const char user_fie[] = " <- User identification       : ";
    size_t     user_len = strlen(user_dat);
    if (user_len == 0) {
        input(stdout, report, user_fie, user_dat, user_len, MIN_USER_LEN,
                MAX_USER_LEN, all);
    } else {
        output(stdout, report, user_fie);
        output(stdout, report, user_dat);
        output(stdout, report, "\n");
    }

    // Request the identification of the company that performs the tests. It is
    // saved so that it is not requested again if more PCBAs are tested during
    // the execution of the program.
    const char comp_fie[] = " <- Company identification    : ";
    size_t     comp_len = strlen(comp_dat);
    if (comp_len == 0) {
        input(stdout, report, comp_fie, comp_dat, comp_len, MIN_COMP_LEN,
                MAX_COMP_LEN, all);
    } else {
        output(stdout, report, comp_fie);
        output(stdout, report, comp_dat);
        output(stdout, report, "\n");
    }
    output(stdout, report, "\n");

    // Request the batch number of the PCBA to be tested.
    const char   bn_fie[] = " <- PCBA batch number         : ";
    size_t       bn_len = strlen(bn_dat);
    input(stdout, report, bn_fie, bn_dat, bn_len, MIN_BN_LEN, MAX_BN_LEN,
            numbers);

    // Request the serial number of the PCBA to be tested.
    const char   sn_fie[] = " <- PCBA serial number        : ";
    size_t       sn_len = strlen(sn_dat);
    input(stdout, report, sn_fie, sn_dat, sn_len, MIN_SN_LEN, MAX_SN_LEN,
            numbers);
}

static void add_csv(const char *user_dat, const char *comp_dat,
        const char *bn_dat, const char *sn_dat, const struct tm *time_struct,
        int all_ok) {
    // If the traceability CSV is empty, add a header with the title of each
    // column.
    fseek(csv, 0, SEEK_END);
    ret = ftell(csv);
    if (ret == 0) {
        fprintf(csv, "\"USER\";\"COMP\";\"B/N\";\"S/N\";\"Time_Date\";\"OK?\"");
        fprintf(csv, "\n");
    }

    // Insert in the traceability CSV the user's identification.
    fprintf(csv, "\"%s\"", user_dat);
    fprintf(csv, ";");

    // Insert in the traceability CSV the company's identification.
    fprintf(csv, "\"%s\"", comp_dat);
    fprintf(csv, ";");

    // Insert in the traceability CSV the batch number of the tested PCBA.
    fprintf(csv, "\"%s\"", bn_dat);
    fprintf(csv, ";");

    // Insert in the traceability CSV the serial number of the tested PCBA.
    fprintf(csv, "\"%s\"", sn_dat);
    fprintf(csv, ";");

    // Insert in the traceability CSV the current time and date.
    char date_short[DEF_SMA_BUF_SIZE] = {0};
    strftime(date_short, sizeof(date_short), "%Y_%m_%d_%H_%M_%S", time_struct);
    fprintf(csv, "\"%s\"", date_short);
    fprintf(csv, ";");

    // Insert in the traceability CSV the whole result of the tests.
    if (all_ok == TRUE) {
        fprintf(csv, "\"Yes\"");
    } else {
        fprintf(csv, "\"No\"");
    }
    fprintf(csv, "\n");
}

// -----------------------------------------------------------------------------

#endif // WIN32
