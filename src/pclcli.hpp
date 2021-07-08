// pCloud Console Client
//
// Copyright (c) 2021 Serghei Iakovlev.
// Copyright (c) 2013-2016 Anton Titov.
// Copyright (c) 2013-2016 pCloud Ltd.
//
// This source file is subject to the New BSD License that is bundled with this
// project in the file LICENSE.
//
// If you did not receive a copy of the license and are unable to obtain it
// through the world-wide-web, please send an email to egrep@protonmail.ch so
// we can send you a copy immediately.

#ifndef PSYNC_PCLCLI_H
#define PSYNC_PCLCLI_H

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include "psynclib.h"
typedef void (*status_callback_t)(int status,  const char * stat_string);

#ifdef __cplusplus
};
#endif

struct pstatus_struct_;

namespace console_client {
  namespace clibrary {
    struct pclcli
    {
      public:

      ~pclcli();
      pclcli();

      // Getters
      const std::string& get_username() {return username_;}
      const std::string& get_password() {return password_;}
      const std::string& get_crypto_pass() {return crypto_pass_;};
      const std::string& get_mount() {return mount_;}

      // Setters
      void set_username(const std::string& arg) { username_ = arg;}
      void set_password(const std::string& arg) { password_ = arg;}
      void set_crypto_pass(const std::string& arg) { crypto_pass_ = arg;};
      void set_mount(const std::string& arg) { mount_ = arg;}
      void set_savepass(bool s) {save_pass_ = s;}
      void setupsetup_crypto(bool p) {setup_crypto_ = p;}
      void set_newuser(bool p) {newuser_ = p;}
      void set_daemon(bool p) {daemon_ = p;}
      void set_status_callback(status_callback_t p) {status_callback_ = p;}

      // Console
      void get_pass_from_console();
      void get_cryptopass_from_console();

      // API calls
      int init();
      static int start_crypto (const char* pass, void * rep);
      static int stop_crypto (const char* path, void * rep);
      static int finalize (const char* path, void * rep);
      static int list_sync_folders (const char* path, void * rep);

      // Singleton
      static pclcli& get_lib();
      char * get_token();
      int logout();
      int unlink();
      int login(const char* user, const char* pass, int save);

      bool crypto_on_;
      bool save_pass_;
      bool setup_crypto_;
      pstatus_struct_* status_;
      bool newuser_;
      status_callback_t status_callback_;
      bool was_init_;

    private:
      std::string username_;
      std::string password_;
      std::string crypto_pass_;
      std::string mount_;

      bool to_set_mount_;
      bool daemon_;

    private:
      void do_get_pass_from_console(std::string& password);
    };
  }
}

#endif  // PSYNC_PCLCLI_H
