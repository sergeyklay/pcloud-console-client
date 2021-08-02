/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "config.h"

#include <stdio.h>

#include "pcontacts.h"
#include "papi.h"
#include "plibs.h"
#include "pnetlibs.h"
#include "pfileops.h"
#include "logger.h"

int do_call_contactlist(result_visitor vis, void *param) {
  psync_socket *sock;
  binresult *bres;
  int i;
  const binresult *contacts;

  if(psync_my_auth[0]) {
    binparam params[] = {P_STR("auth", psync_my_auth)};
    sock = psync_apipool_get();
    if (sock)
      bres = send_command(sock, "contactlist", params);
    else return -2;
  } else if (psync_my_user && psync_my_pass) {
    binparam params[] = {P_STR("username", psync_my_user), P_STR("password", psync_my_pass)};
    sock = psync_apipool_get();
    if (sock)
      bres = send_command(sock, "contactlist", params);
    else return -2;
  } else return -1;
  if (likely(bres))
    psync_apipool_release(sock);
  else {
    psync_apipool_release_bad(sock);
    log_error("command \"contacts\" returned invalid result");
    return -1;
  }

  contacts = psync_find_result(bres, "contacts", PARAM_ARRAY);

  if (!contacts->length) {
    psync_free(bres);
    log_error("command \"contacts\" returned empty result");
    return -2;
  } else {
    psync_sql_start_transaction();
    psync_sql_statement("DELETE FROM contacts");
    for (i = 0; i < contacts->length; ++i)
       vis(i, contacts->array[i], param);
    psync_sql_commit_transaction();
  }

  psync_free(bres);
  return 0;
}

static void insert_cache_contact(int i, const binresult *user, void *_this) {
  const char *char_field = 0;
  uint64_t id = 0;
  psync_sql_res *q;

  char_field = psync_find_result(user, "name", PARAM_STR)->str;
  id = psync_find_result(user, "source", PARAM_NUM)->num;
  if ((id == 1) || (id == 3)) {
    q=psync_sql_prep_statement("REPLACE INTO contacts  (mail) VALUES (?)");
    char_field = psync_find_result(user, "value", PARAM_STR)->str;
    psync_sql_bind_lstring(q, 1, char_field, strlen(char_field));
    psync_sql_run_free(q);
  }
}

void cache_contacts() {
  do_call_contactlist(&insert_cache_contact, NULL);
}


static int create_contact(psync_list_builder_t *builder, void *element, psync_variant_row row) {
  contact_info_t *contact;
  const char *str;
  size_t len;
  contact=(contact_info_t *)element;
  str=psync_get_lstring(row[0], &len);
  contact->mail=str;
  psync_list_add_lstring_offset(builder, offsetof(contact_info_t, mail), len);
  if (row[1].type != PSYNC_TNULL)
    str=psync_get_lstring(row[1], &len);
  else
    str = "";
  contact->name=str;
  psync_list_add_lstring_offset(builder, offsetof(contact_info_t, name), len);
  contact->teamid=psync_get_number(row[2]);
  contact->type=psync_get_number(row[3]);
  return 0;
}

pcontacts_list_t *do_psync_list_contacts() {
  psync_list_builder_t *builder;
  psync_sql_res *res;
  builder=psync_list_builder_create(sizeof(contact_info_t), offsetof(pcontacts_list_t, entries));
  res=psync_sql_query_rdlock("select mail, ifnull(name, ' ') , 0 as teamid, 1 as type from contacts "
                             "union all "
                             "select  mail, (firstname||' '||lastname) as name, 0 as teamid , 2 as type from baccountemail "
                             "union all "
                             "select  '' as mail, name , id as teamid, 3 as type from baccountteam "
                             "ORDER BY name "
  );
  psync_list_bulder_add_sql(builder, res, create_contact);

  return (pcontacts_list_t *)psync_list_builder_finalize(builder);
}

pcontacts_list_t *do_psync_list_myteams() {
  psync_list_builder_t *builder;
  psync_sql_res *res;
  builder=psync_list_builder_create(sizeof(contact_info_t), offsetof(pcontacts_list_t, entries));
  res=psync_sql_query_rdlock("SELECT  '' AS mail, name , id AS teamid, 3 AS type FROM myteams "
                             "ORDER BY name "
  );
  psync_list_bulder_add_sql(builder, res, create_contact);

  return (pcontacts_list_t *)psync_list_builder_finalize(builder);
}

static void process_shares_out(const binresult *shares_out, int shcnt) {
  const binresult *share;
  const binresult *br;
  psync_sql_res *q;
  int i, isincomming =  0;
  uint64_t folderowneruserid, owneruserid;


  for (i = 0; i < shcnt; ++i) {
    share = shares_out->array[i];

    folderowneruserid =  psync_find_result(share, "folderowneruserid", PARAM_NUM)->num;
	psync_get_current_userid(&owneruserid);
    isincomming = (folderowneruserid == owneruserid) ? 0 : 1;

    q=psync_sql_prep_statement("REPLACE INTO sharedfolder (id, folderid, ctime, permissions, userid, mail, name, isincoming) "
                                                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    psync_sql_bind_uint(q, 1, psync_find_result(share, "shareid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 2, psync_find_result(share, "folderid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 3, psync_find_result(share, "created", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 4, psync_get_permissions(share));
    psync_sql_bind_uint(q, 5, psync_find_result(share, "touserid", PARAM_NUM)->num);
    br=psync_find_result(share, "tomail", PARAM_STR);
    psync_sql_bind_lstring(q, 6, br->str, br->length);
    if(!(br=psync_check_result(share, "foldername", PARAM_STR)))
      br=psync_check_result(share, "sharename", PARAM_STR);
    psync_sql_bind_lstring(q, 7, br->str, br->length);
    psync_sql_bind_uint(q, 8, isincomming);
    psync_sql_run_free(q);
  }
}

static void process_shares_in(const binresult *shares_in, int shcnt) {
  const binresult *share;
  const binresult *br;
  psync_sql_res *q;
  int i;

  for (i = 0; i < shcnt; ++i) {
    share = shares_in->array[i];

    q=psync_sql_prep_statement("REPLACE INTO sharedfolder (id, isincoming, folderid, ctime, permissions, userid, mail, name) "
                                                  "VALUES (?, 1, ?, ?, ?, ?, ?, ?)");
    psync_sql_bind_uint(q, 1, psync_find_result(share, "shareid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 2, psync_find_result(share, "folderid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 3, psync_find_result(share, "created", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 4, psync_get_permissions(share));
    psync_sql_bind_uint(q, 5, psync_find_result(share, "fromuserid", PARAM_NUM)->num);
    br=psync_find_result(share, "frommail", PARAM_STR);
    psync_sql_bind_lstring(q, 6, br->str, br->length);
    if(!(br=psync_check_result(share, "foldername", PARAM_STR)))
      br=psync_check_result(share, "sharename", PARAM_STR);
    psync_sql_bind_lstring(q, 7, br->str, br->length);
    psync_sql_run_free(q);
  }
}

static void process_shares_req_out(const binresult *shares_out, int shcnt) {
  const binresult *share;
  const binresult *br;
  psync_sql_res *q;
  int i, isincomming =  0;
  uint64_t folderowneruserid, owneruserid;


  for (i = 0; i < shcnt; ++i) {
    share = shares_out->array[i];

    folderowneruserid =  psync_find_result(share, "folderowneruserid", PARAM_NUM)->num;
    psync_get_current_userid(&owneruserid);
    isincomming = (folderowneruserid == owneruserid) ? 0 : 1;

    q=psync_sql_prep_statement("REPLACE INTO sharerequest (id, folderid, ctime, etime, permissions, userid, mail, name, message,  isincoming, isba) "
                                                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    psync_sql_bind_uint(q, 1, psync_find_result(share, "sharerequestid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 2, psync_find_result(share, "folderid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 3, psync_find_result(share, "created", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 4, psync_find_result(share, "expires", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 5, psync_get_permissions(share));
    psync_sql_bind_uint(q, 6, folderowneruserid);
    br=psync_find_result(share, "tomail", PARAM_STR);
    psync_sql_bind_lstring(q, 7, br->str, br->length);
    if(!(br=psync_check_result(share, "foldername", PARAM_STR)))
      br=psync_check_result(share, "sharename", PARAM_STR);
    psync_sql_bind_lstring(q, 8, br->str, br->length);
    br=psync_check_result(share, "message", PARAM_STR);
    if (br)
      psync_sql_bind_lstring(q, 9, br->str, br->length);
    else
      psync_sql_bind_null(q, 9);
    psync_sql_bind_uint(q, 10, isincomming);
    psync_sql_bind_uint(q, 11, isincomming);
    psync_sql_run_free(q);
  }
}

static void process_shares_req_in(const binresult *shares_in, int shcnt) {
  const binresult *share;
  const binresult *br;
  psync_sql_res *q;
  int i, isincomming = 1;
  uint64_t folderowneruserid, owneruserid;

  for (i = 0; i < shcnt; ++i) {
    share = shares_in->array[i];
  br = psync_check_result(share, "folderowneruserid", PARAM_NUM);
  if (br) {
    folderowneruserid = br->num;
    psync_get_current_userid(&owneruserid);
    isincomming = (folderowneruserid == owneruserid) ? 0 : 1;
  }

    q=psync_sql_prep_statement("REPLACE INTO sharerequest (id, folderid, ctime, etime, permissions, userid, mail, name, message, isincoming, isba) "
                                                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    psync_sql_bind_uint(q, 1, psync_find_result(share, "sharerequestid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 2, psync_find_result(share, "folderid", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 3, psync_find_result(share, "created", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 4, psync_find_result(share, "expires", PARAM_NUM)->num);
    psync_sql_bind_uint(q, 5, psync_get_permissions(share));
    psync_sql_bind_uint(q, 6, psync_find_result(share, "fromuserid", PARAM_NUM)->num);
    br=psync_find_result(share, "frommail", PARAM_STR);
    psync_sql_bind_lstring(q, 7, br->str, br->length);
    if(!(br=psync_check_result(share, "foldername", PARAM_STR)))
      br=psync_check_result(share, "sharename", PARAM_STR);
    psync_sql_bind_lstring(q, 8, br->str, br->length);
    br=psync_check_result(share, "message", PARAM_STR);
    if (br)
      psync_sql_bind_lstring(q, 9, br->str, br->length);
    else
      psync_sql_bind_null(q, 9);
    psync_sql_bind_uint(q, 10, isincomming);
    psync_sql_bind_uint(q, 11, !isincomming);
    psync_sql_run_free(q);
  }
}


void cache_shares() {
  psync_socket *api;
  binresult *bres;
  uint64_t result;
  const char *errorret;
  const binresult *array, *shares;
  int shcnt;
  psync_sql_res *q;

  if(psync_my_auth[0]) {
    binparam params[] = {P_STR("auth", psync_my_auth), P_STR("timeformat", "timestamp")};
    api = psync_apipool_get();
    if (unlikely(!api)) {
      log_warn("Can't get api from the pool. No pool?");
      return;
    }
    bres = send_command(api, "listshares", params);
  } else if (psync_my_user && psync_my_pass) {
    binparam params[] = {P_STR("username", psync_my_user), P_STR("password", psync_my_pass), P_STR("timeformat", "timestamp")};
    api = psync_apipool_get();
    if (unlikely(!api)) {
      log_warn("Can't get api from the pool. No pool?");
      return;
    }
    bres = send_command(api, "listshares", params);
  } else return;
  if (likely(bres))
    psync_apipool_release(api);
  else {
    psync_apipool_release_bad(api);
    log_warn("Send command returned in valid result");
    return;
  }
  result=psync_find_result(bres, "result", PARAM_NUM)->num;
  if (unlikely(result)) {
    errorret = psync_find_result(bres, "error", PARAM_STR)->str;
    log_warn("command listshares returned error code %u message %s", (unsigned)result, errorret);
    psync_process_api_error(result);
    psync_free(bres);
    return;
  }

  shares = psync_find_result(bres, "shares", PARAM_HASH);

  psync_sql_start_transaction();

  q=psync_sql_prep_statement("DELETE FROM sharerequest ");
  psync_sql_run_free(q);

  q=psync_sql_prep_statement("DELETE FROM sharedfolder ");
  psync_sql_run_free(q);

  array=psync_find_result(shares, "outgoing", PARAM_ARRAY);

  shcnt = array->length;
  if (shcnt) {
    process_shares_out(array, shcnt);
  }

  array=psync_find_result(shares, "incoming", PARAM_ARRAY);

  shcnt = array->length;
  if (shcnt) {
    process_shares_in(array, shcnt);
  }

  shares = psync_find_result(bres, "requests", PARAM_HASH);
  array=psync_find_result(shares, "outgoing", PARAM_ARRAY);

  shcnt = array->length;
  if (shcnt) {
    process_shares_req_out(array, shcnt);
  }

  array=psync_find_result(shares, "incoming", PARAM_ARRAY);

  shcnt = array->length;
  if (shcnt) {
    process_shares_req_in(array, shcnt);
  }

  psync_sql_commit_transaction();

  psync_free(bres);

}
