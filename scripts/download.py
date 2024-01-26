#!/usr/bin/env python3

"""This script downloads the latest httplib.h file from the source repository."""

from tqdm import tqdm
import requests
import json

# Establish connection to download the httplib.h file from github
url = "https://github.com/yhirose/cpp-httplib/blob/master/httplib.h"
response = requests.get(url, stream=True)

# Write the contents of the response to a temporary byte array (with progress bar)
raw_data = bytearray()
for data in tqdm(response.iter_content()):
  raw_data.extend(data)

# Parse the JSON response then extract the httplib.h file
data_obj = json.loads(raw_data.decode("utf-8"))
file_lines = data_obj["payload"]["blob"]["rawLines"]

# Write the httplib.h file to a temporary file
temp_file = open("httplib.h.temp", "w")
temp_file.write("\n".join(file_lines))
temp_file.close()
