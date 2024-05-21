# Author(s):
#   Daniel Opara <hello@danielopara.me>

from copy import deepcopy

"""This script prints out how to organize your bitwords given the names and 
length of your bitfields. This was used to determine the bitfield schema generated in 
bitfield_generator.py.
"""

# Change these values to reflect the number of bits for your project!

# you could use the header field to include the bits that will be included in every fragment you transmit
# the rfm96 can handle package up to 252 bytes so we can transmit all we need in one go
# you might just end up playing around with this to get something you want
HEADER_FIELD = 0
WORD_SIZE = 64


def distribute_bitfields(
    bitfields,
    max_value_per_bucket,
    swap_indices=None,
    insert_indices=None,
    swap_first=False,
):
    # sort bit field values in descending order
    sorted_bitfields = sorted(bitfields, key=lambda x: x[1], reverse=True)
    before_swaps = deepcopy(sorted_bitfields)

    # sometimes descending order isnt the best way to iterating through the array
    # and organize bit, we can swap/insert indicies if we want to add fields in
    # buckets in a particular order

    # recommended to use these whenever you can because descending order can be a
    # bitch to deal with when it comes to this shit

    if swap_first:
        if swap_indices:
            for swap_index1, swap_index2 in swap_indices:
                sorted_bitfields[swap_index1], sorted_bitfields[swap_index2] = (
                    sorted_bitfields[swap_index2],
                    sorted_bitfields[swap_index1],
                )
        if insert_indices:
            for insert_elem, insert_idx in insert_indices:
                elem_to_insert = sorted_bitfields[insert_elem]
                sorted_bitfields.pop(insert_elem)
                sorted_bitfields.insert(insert_idx, elem_to_insert)
    else:
        if insert_indices:
            for insert_elem, insert_idx in insert_indices:
                elem_to_insert = sorted_bitfields[insert_elem]
                sorted_bitfields.pop(insert_elem)
                sorted_bitfields.insert(insert_idx, elem_to_insert)
        if swap_indices:
            for swap_index1, swap_index2 in swap_indices:
                sorted_bitfields[swap_index1], sorted_bitfields[swap_index2] = (
                    sorted_bitfields[swap_index2],
                    sorted_bitfields[swap_index1],
                )

    print("After Sorting\t\t\t\t\t\tAfter Swaps and Inserts")
    for i, (before, after) in enumerate(zip(before_swaps, sorted_bitfields)):
        print(f"{i}: {before}\t\t\t\t\t\t{after}")

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
    ("gps coord: lat", 28),  # takes into account single sign bit
    ("gps fix", 1),
    ("external temp", 11),
    ("internal temp: avbay", 11),
    ("internal temp: engine bay", 11),
    ("gps signal quality", 2),
    ("gps antenna status", 2),
    ("gps coord: long", 29),  # takes into account single sign bit
    ("failure flag", 10),
    ("gps altitude", 15),
    ("altitude", 15),
    ("vertical velo", 15),
    ("z accel", 11),
    ("y accel", 9),
    ("x accel", 9),
    ("mag y", 11),
    ("mag z", 11),
    ("mag x", 11),
    ("x gyro", 20),
    ("y gyro", 20),
    ("z gyro", 17),
    ("gps speed", 10),
    ("gps link", 3),
    ("state", 4),
    ("time", 25),
]

swapper = [
    (13, 22),
]

inserter = [(20, 0), (21, 0), (18, 6), (20, 7), (20, 11), (23, 12), (22, 13), (24, 18)]


bitfield_vals, bitfield_names = distribute_bitfields(
    bitfield_list, WORD_SIZE, swapper, inserter
)

print()
print(
    f"NOTE: THE FOLLOWING IS A SUGGESTION AS TO HOW TO ORGANIZE THE BITFIELD, IT IS RECOMMENDED THAT YOU SWITCH FIELD POSITIONS AS YOU SEE FIT"
)
print(f"Arrange your {WORD_SIZE} bit words according to the buckets shown below:")
print("-------------------------------------------------------------------")
# used gpt for this printing part
for i, (vals, names) in enumerate(zip(bitfield_vals, bitfield_names)):
    # Print left-justified values and names
    # was a little scuffed so had to subtract 55, should be good all around tho, change the number as you please
    print(
        f"Bucket {i + 1}: {vals}{' ' * (len(str(bitfield_vals)) - 80 - len(str(vals)))}  {names}"
    )

# ngl this tool is really confusing and maybe a little bad
