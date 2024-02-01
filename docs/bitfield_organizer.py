# Author(s):
#   Daniel Opara <hello@danielopara.me>

from copy import deepcopy

"""This script prints out how to organize your bitwords given the names and 
length of your bitfields. This was used to determine the bitfield schema generated in 
bitfield_generator.py.
"""

# Change these values to reflect your project!
HEADER_FIELD = 8
WORD_SIZE = 32


def distribute_bitfields(bitfields, max_value_per_bucket, swap_indices=None):
    # sort bit field values in descending order
    sorted_bitfields = sorted(bitfields, key=lambda x: x[1], reverse=True)
    before_swaps = deepcopy(sorted_bitfields)

    # sometimes descending order isnt the best way to iterating through the array
    # and organize bit, we can swap indicies if we want to add some field in a
    # bucket in a particular order
    if swap_indices:
        for swap_index1, swap_index2 in swap_indices:
            sorted_bitfields[swap_index1], sorted_bitfields[swap_index2] = (
                sorted_bitfields[swap_index2],
                sorted_bitfields[swap_index1],
            )

    print("Before Sorting\t\t\t\t\tAfter Sorting")
    for before, after in zip(before_swaps, sorted_bitfields):
        print(f"{before}\t\t\t\t\t{after}")

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

        # if the field can't fit in existing buckets, distribute it across multiple buckets
        if not field_added:
            remaining_value = field_value
            if sum(buckets[-1]) == max_value_per_bucket:
                new_bucket = [HEADER_FIELD, remaining_value]
                new_bucket_name = ["HEADER_FIELD", field_name]
                buckets.append(new_bucket)
                bucket_names.append(new_bucket_name)
            else:
                while remaining_value > 0:
                    # create a new bucket for the remaining part of the field
                    new_bucket = [HEADER_FIELD]
                    new_bucket_name = ["HEADER_FIELD", field_name]

                    # check if there's some space in the prev bucket
                    # if there is, put it there
                    if sum(buckets[-1]) < max_value_per_bucket:
                        # determine how much of the field can fit in the prev bucket
                        partial_prev = max_value_per_bucket - sum(buckets[-1])
                        partial_new = remaining_value - partial_prev

                        # add the partial fields to the prev bucket and new buckets
                        new_bucket.append(partial_new)
                        buckets[-1].append(partial_prev)
                        bucket_names[-1].append(field_name)

                        remaining_value -= partial_prev + partial_new

                        buckets.append(new_bucket)
                        bucket_names.append(new_bucket_name)

    return buckets, bucket_names


# list of bitfields and their values, replace with your bitfields as needed
bitfield_list = [
    ("gps coord: lat", 28),
    ("num satellites", 3),
    ("external temp", 14),
    ("internal temp: avbay", 12),
    ("internal temp: engine bay", 12),
    ("velocity", 9),
    ("altitude", 17),
    ("accel x", 14),
    ("accel y", 14),
    ("accel z", 14),
    ("gyro x", 23),
    ("gyro y", 23),
    ("gyro z", 16),
    ("mag x", 11),
    ("mag y", 11),
    ("mag z", 11),
    ("gps coord: long", 29),
    ("failure flag", 13),
]

swapper = [
    (0, 1),
]

bitfield_vals, bitfield_names = distribute_bitfields(bitfield_list, WORD_SIZE, swapper)

print()
print(f"Arrange your {WORD_SIZE} bit words according to the buckets shown below:")
print("-------------------------------------------------------------------")
# used gpt for this printing part
for i, (vals, names) in enumerate(zip(bitfield_vals, bitfield_names)):
    # Print left-justified values and names
    # was a little scuffed so had to subtract 55, should be good all around tho, change the number as you please
    print(
        f"Bucket {i + 1}: {vals}{' ' * (len(str(bitfield_vals)) - 120 - len(str(vals)))}  {names}"
    )
