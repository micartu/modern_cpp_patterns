#!/usr/bin/env /usr/bin/python3

import sys
import io
import datetime


if __name__ == '__main__':
    if len(sys.argv) > 3:
        ids = read_services(sys.argv[1], *read_start_ids(sys.argv[2]))
        write_start_ids(sys.argv[2], *ids)
    else:
        print("Usage: " + sys.argv[0] + " <boost_download_url> <boost_installation_dir> <where_to_place_extracted_sub_boost>")
