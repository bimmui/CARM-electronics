# Author(s):
#   Daniel Opara <hello@danielopara.me>

"""This script prints out how to organize your bitwords given the names and 
length of your bitfields. This was used to determine the bitfield schema generated in 
bitfield_generator.py.
"""

# Change these values to reflect your project!
HEADER_FIELD = 4
WORD_SIZE = 32


def distribute_bitfields(bitfields, max_value_per_bucket):
    # sort bit field values in descending order
    sorted_bitfields = sorted(bitfields, key=lambda x: x[1], reverse=True)

    # initialize a single word with the value of the header field
    buckets = [[HEADER_FIELD]]
    bucket_names = [["HEADER_FIELD"]]

    # distribute fields into buckets
    for field_name, field_value in sorted_bitfields:
        field_added = False

        # try to add the field to an existing bucket
        for i, bucket in enumerate(buckets):
            if sum(bucket) + field_value <= max_value_per_bucket:
                bucket.append(field_value)
                bucket_names[i].append(field_name)
                field_added = True
                break

        # if the field canit fit in existing buckets, make new bucket
        if not field_added:
            buckets.append([HEADER_FIELD, field_value])
            bucket_names.append(["HEADER_FIELD", field_name])

    return buckets, bucket_names


# list of bitfields and their values, replace with your bitfields as needed
bitfield_list = [
    ("time", 8),
    ("external temp", 14),
    ("internal temp: avbay", 12),
    ("internal temp: engine bay", 12),
    ("altitude", 17),
    ("barometric pressure", 15),
    ("acceleration", 18),
    ("gyro shit", 10),
    ("gps coord: latitude", 28),
    ("gps coord: longitude", 29),
]

bitfield_vals, bitfield_names = distribute_bitfields(bitfield_list, WORD_SIZE)

print()
print(f"Arrange your {WORD_SIZE} bit words according to the buckets shown below:")
print("-------------------------------------------------------------------")
# used gpt for this printing part
for i, (vals, names) in enumerate(zip(bitfield_vals, bitfield_names)):
    # Print left-justified values and names
    # was a little scuffed so had to subtract 55, should be good all around tho
    print(
        f"Bucket {i + 1}: {vals}{' ' * (len(str(bitfield_vals)) - 55 - len(str(vals)))}  {names}"
    )
