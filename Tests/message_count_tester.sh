#!/bin/bash

# Run the program and capture the output in a variable
output=$(../ProdConsOut config1)

# Print the program output
echo "Program Output:"
echo "$output"
echo ""

# Initialize counts
declare -A producer_counts
declare -A type_counts

# Process the captured output
while IFS= read -r line
do
    if [[ $line == "DONE" ]]; then
        break
    fi

    # Parse the line
    read -r _ producer_id type _ <<< "$line"

    # Increment producer count
    producer_counts["Producer $producer_id"]=$((producer_counts["Producer $producer_id"] + 1))

    # Increment type count
    type_counts["Producer $producer_id $type"]=$((type_counts["Producer $producer_id $type"] + 1))
done <<< "$output"

# Print the results
echo "Message Counts by Producer and Type:"
for producer in "${!producer_counts[@]}"
do
    echo "$producer: ${producer_counts[$producer]}"
    for type in NEWS SPORTS WEATHER
    do
        count=${type_counts["$producer $type"]}
        if [[ -n $count ]]; then
            echo "  $type: $count"
        else
            echo "  $type: 0"
        fi
    done
done
