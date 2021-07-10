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

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <cstring>

#include "psynclib.h"
#include "pcompat.h"

#include "pclcli.hpp"
#include "version.hpp"

namespace cc = console_client;
namespace clib = cc::clibrary;

clib::pclcli& clib::pclcli::get_lib() {
  static clib::pclcli g_lib;
  return g_lib;
}

// TODO: See note in 'clib::pclcli::init'
// static std::string exec(const char* cmd) {
//   std::array<char, 128> buffer{};
//   std::string result;
//   size_t size = 0;
//
//   std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
//   if (!pipe) {
//     throw std::runtime_error("popen() failed");
//   }
//
//   while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
//     result += buffer.data();
//   }
//
//   return result;
// }

char * clib::pclcli::get_token(){
  return psync_get_token();
}

void clib::pclcli::get_pass_from_console()
{
  do_get_pass_from_console(password_);
}

void clib::pclcli::get_cryptopass_from_console()
{
  do_get_pass_from_console(crypto_pass_);
}

void clib::pclcli::do_get_pass_from_console(std::string& password)
{
  if (daemon_) {
     std::cout << "Not able to read password when started as daemon." << std::endl;
     exit(1);
  }
#ifdef P_OS_POSIX
  termios oldt{};
  tcgetattr(STDIN_FILENO, &oldt);
  termios newt = oldt;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  std::cout << "Please, enter password" << std::endl;
  getline(std::cin, password);
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#else
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD modeoff;
  DWORD modeon;
  DWORD mode;
  GetConsoleMode(hStdin, &mode);
    modeoff = mode & ~ENABLE_ECHO_INPUT;
    modeon = mode | ENABLE_ECHO_INPUT;
  SetConsoleMode(hStdin, modeoff);
  std::cout << "Please, enter password" << std::endl;
  getline(std::cin, password);
  SetConsoleMode(hStdin, modeon);
#endif
}

void event_handler(psync_eventtype_t event, psync_eventdata_t eventdata){
 if (event<PEVENT_FIRST_USER_EVENT){
    if (event&PEVENT_TYPE_FOLDER)
      std::cout <<"folder event=" << event<<", syncid="<< eventdata.folder->syncid<<", folderid="<<eventdata.folder->folderid<<", name="
         <<eventdata.folder->name<<", local="<<eventdata.folder->localpath<<", remote="<< eventdata.folder->remotepath<<std::endl;
    else
      std::cout <<"file event=" << event<<", syncid="<< eventdata.folder->syncid<<", file="<<eventdata.file->fileid<<", name="
         << eventdata.file->name<<", local="<<eventdata.file->localpath<<", remote="<< eventdata.file->remotepath<<std::endl;
  }
 else if (event>=PEVENT_FIRST_SHARE_EVENT)
    std::cout <<"share event=" << event<<", folderid="<< eventdata.share->folderid<<", sharename="<<eventdata.share->sharename<<
                    ", email="<< eventdata.share->toemail<<", message="<<eventdata.share->message<<", userid="<< eventdata.share->userid<<
                    ", shareid="<<eventdata.share->shareid<<", sharerequestid="<<eventdata.share->sharerequestid<<
                    ", created="<<eventdata.share->created<<", canread="<<eventdata.share->canread<<", cancreate="<<eventdata.share->cancreate<<
                    ", canmodify="<<eventdata.share->canmodify<<", candelete="<<eventdata.share->candelete<<std::endl;
 else
    std::cout <<"event" << event << std::endl;
}

static int lib_setup_crypto() {
  int ret = 0;
  ret = psync_crypto_issetup();
  if (ret) {
    ret = psync_crypto_start(clib::pclcli::get_lib().get_crypto_pass().c_str());
    std::cout << "crypto is setup, login result=" << ret << std::endl;
  } else {
    std::cout << "crypto is not setup" << std::endl;
    ret = psync_crypto_setup(clib::pclcli::get_lib().get_crypto_pass().c_str(), "no hint");
    if (ret)
      std::cout << "crypto setup failed" << std::endl;
    else{
      ret = psync_crypto_start(clib::pclcli::get_lib().get_crypto_pass().c_str());
      std::cout << "crypto setup successful, start=" << ret << std::endl;
      ret =  psync_crypto_mkdir(0, "Crypto", nullptr, nullptr) ;
      std::cout << "creating folder=" << ret << std::endl;
    }
  }
  return ret;
}

static char const * status2string (uint32_t status){
  switch (status) {
    case PSTATUS_READY: return "READY";
    case PSTATUS_DOWNLOADING: return "DOWNLOADING";
    case PSTATUS_UPLOADING: return "UPLOADING";
    case PSTATUS_DOWNLOADINGANDUPLOADING: return "DOWNLOADINGANDUPLOADING";
    case PSTATUS_LOGIN_REQUIRED: return "LOGIN_REQUIRED";
    case PSTATUS_BAD_LOGIN_DATA: return "BAD_LOGIN_DATA";
    case PSTATUS_BAD_LOGIN_TOKEN : return "BAD_LOGIN_TOKEN";
    case PSTATUS_ACCOUNT_FULL: return "ACCOUNT_FULL";
    case PSTATUS_DISK_FULL: return "DISK_FULL";
    case PSTATUS_PAUSED: return "PAUSED";
    case PSTATUS_STOPPED: return "STOPPED";
    case PSTATUS_OFFLINE: return "OFFLINE";
    case PSTATUS_CONNECTING: return "CONNECTING";
    case PSTATUS_SCANNING: return "SCANNING";
    case PSTATUS_USER_MISMATCH: return "USER_MISMATCH";
    case PSTATUS_ACCOUT_EXPIRED: return "ACCOUT_EXPIRED";
    default :return "Unrecognized status";
  }
}

static void status_change(pstatus_t* status) {
  static int cryptocheck=0;
  static int mount_set=0;
  std::cout << "Down: " <<  status->downloadstr << "| Up: " << status->uploadstr <<", status is " << status2string(status->status) << std::endl;
  *clib::pclcli::get_lib().status_ = *status;
  if (status->status==PSTATUS_LOGIN_REQUIRED){
    if (clib::pclcli::get_lib().get_password().empty())
      clib::pclcli::get_lib().get_pass_from_console();
    psync_set_user_pass(clib::pclcli::get_lib().get_username().c_str(), clib::pclcli::get_lib().get_password().c_str(), (int) clib::pclcli::get_lib().save_pass_);
    std::cout << "logging in" << std::endl;
  }
  else if (status->status==PSTATUS_BAD_LOGIN_DATA){
    if (!clib::pclcli::get_lib().newuser_) {
      clib::pclcli::get_lib().get_pass_from_console();
      psync_set_user_pass(clib::pclcli::get_lib().get_username().c_str(), clib::pclcli::get_lib().get_password().c_str(), (int) clib::pclcli::get_lib().save_pass_);
    }
    else {
    std::cout << "registering" << std::endl;
    if (psync_register(clib::pclcli::get_lib().get_username().c_str(), clib::pclcli::get_lib().get_password().c_str(),1, nullptr)){
      std::cout << "both login and registration failed" << std::endl;
      exit(1);
    }
    else{
      std::cout << "registered, logging in" << std::endl;
      psync_set_user_pass(clib::pclcli::get_lib().get_username().c_str(), clib::pclcli::get_lib().get_password().c_str(), (int) clib::pclcli::get_lib().save_pass_);
    }

    }
  }
  if (status->status==PSTATUS_READY || status->status==PSTATUS_UPLOADING || status->status==PSTATUS_DOWNLOADING || status->status==PSTATUS_DOWNLOADINGANDUPLOADING){
    if (!cryptocheck){
      cryptocheck=1;
      if (clib::pclcli::get_lib().setup_crypto_) {
        lib_setup_crypto();
      }
    }
    psync_fs_start();
  }
  if (clib::pclcli::get_lib().status_callback_)
    clib::pclcli::get_lib().status_callback_((int)status->status, status2string(status->status));
}

int clib::pclcli::start_crypto (const char* pass, void * rep) {
  std::cout << "calling startcrypto pass: "<<pass << std::endl;
  get_lib().crypto_pass_ = pass;
  return lib_setup_crypto();
}

int clib::pclcli::stop_crypto (const char* path, void * rep) {
  psync_crypto_stop();
  get_lib().crypto_on_ = false;
}

int clib::pclcli::finalize (const char* path, void * rep) {
  psync_destroy();
  exit(0);
}

int clib::pclcli::list_sync_folders (const char* path, void * rep) {
  psync_folder_list_t * folders = psync_get_sync_list();
  rep = psync_malloc(sizeof(folders));
  memcpy(rep, folders, sizeof(folders));
}

int clib::pclcli::init() {
  // TODO: Old behavior leads to char overflow. Try to sort out
  // const std::string client_name = " Console Client v.2.0.1";
  // std::string software_string = exec("lsb_release -ds");
  // psync_set_software_string(software_string.append(client_name).c_str());

  psync_set_software_string(PCLSYNC_VERSION_FULL);
  if (setup_crypto_ && crypto_pass_.empty())
    return 3;

  if (psync_init()) {
    std::cout <<"init failed\n"; // TODO: Describe why
    return 1;
  }

   was_init_ = true;
   if (!get_mount().empty())
    psync_set_string_setting("fsroot", get_mount().c_str());

  psync_start_sync(status_change, event_handler);
  char * username_old = psync_get_username();

  if (username_old){
    if (username_ != username_old){
      std::cout << "logged in with user " << username_old <<", not "<< username_ <<", unlinking"<<std::endl;
      psync_unlink();
      psync_free(username_old);
      return 2;
    }
    psync_free(username_old);
  }

  psync_add_overlay_callback(20, &clib::pclcli::start_crypto);
  psync_add_overlay_callback(21,&clib::pclcli::stop_crypto);
  psync_add_overlay_callback(22,&clib::pclcli::finalize);
  psync_add_overlay_callback(23,&clib::pclcli::list_sync_folders);

  return 0;
}

int clib::pclcli::login(const char* user, const char* pass, int save) {
  set_username(user);
  set_password(pass);
  set_savepass(bool(save));
  psync_set_user_pass(user,pass, save);
  return 0;
}

int clib::pclcli::logout () {
  set_password("");
  psync_logout();
  return 0;
}

int clib::pclcli::unlink () {
  set_username("");
  set_password("");
  psync_unlink();
  return 0;
}

clib::pclcli::pclcli():
  crypto_on_(false),
  save_pass_(false),
  newuser_(false),
  to_set_mount_(false),
  daemon_(false),
  status_(new pstatus_struct_()),
  was_init_(false),
  setup_crypto_(false),
  status_callback_{} {}

clib::pclcli::~pclcli() = default;
