#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "protocol/req_id.h"
#include "common/log.h"

namespace srpc {


static int g_req_id_length = 20;
static int g_random_fd = -1;

static thread_local std::string t_req_id_no;
static thread_local std::string t_max_req_id_no;

std::string ReqIDUtil::GenReqID() {
  if (t_req_id_no.empty() || t_req_id_no == t_max_req_id_no) {
    if (g_random_fd == -1) {
      g_random_fd = open("/dev/urandom", O_RDONLY);
    }
    std::string res(g_req_id_length, 0);
    if ((read(g_random_fd, &res[0], g_req_id_length)) != g_req_id_length) {
      ERRORLOG("read form /dev/urandom error");
      return "";
    }
    for (int i = 0; i < g_req_id_length; ++i) {
      uint8_t x = ((uint8_t)(res[i])) % 10;
      res[i] = x + '0';
      t_max_req_id_no += "9";
    }
    t_req_id_no = res;
  } else {
    size_t i = t_req_id_no.length() - 1;
    while (t_req_id_no[i] == '9' && i >= 0) {
      i--;
    }
    if (i >= 0) {
      t_req_id_no[i] += 1;
      for (size_t j = i + 1; j < t_req_id_no.length(); ++j) {
        t_req_id_no[j] = '0';
      }
    }
  }

  return t_req_id_no;

}

}