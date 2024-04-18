class TrieNode:
    def __init__(self):
        self.children = [None] * 26
        self.out = 0
        self.failure = None

def build_matching_machine(patterns):
    root = TrieNode()
    queue = []
    
    # Construct the Trie
    for pattern in patterns:
        current = root
        for char in pattern:
            index = ord(char) - ord('a')
            if not current.children[index]:
                current.children[index] = TrieNode()
            current = current.children[index]
        current.out |= (1 << len(queue))
        queue.append(current)
    
    # Initialize failure function
    for i in range(26):
        if root.children[i]:
            root.children[i].failure = root
            queue.append(root.children[i])
        else:
            root.children[i] = root
    
    # Construct failure function using BFS
    while queue:
        current = queue.pop(0)
        for i in range(26):
            if current.children[i]:
                failure = current.failure
                while not failure.children[i]:
                    failure = failure.failure
                failure = failure.children[i]
                current.children[i].failure = failure
                current.children[i].out |= failure.out
                queue.append(current.children[i])
    
    return root

def find_next_state(current_state, char):
    index = ord(char) - ord('a')
    while not current_state.children[index]:
        current_state = current_state.failure
    return current_state.children[index]

def search_words(patterns, text):
    root = build_matching_machine(patterns)
    current_state = root
    matches = []

    # Traverse the text and find matches
    for i, char in enumerate(text):
        current_state = find_next_state(current_state, char)
        if current_state.out == 0:
            continue
        
        # Match found, store all matching words
        for j, pattern in enumerate(patterns):
            if current_state.out & (1 << j):
                matches.append((pattern, i - len(pattern) + 1, i))
    
    return matches

def main():
    # Read patterns from file
    with open("patterns.txt", "r") as f:
        patterns = [line.strip() for line in f]

    # Read text from file
    with open("text.txt", "r") as f:
        text = f.read().strip()
    
    matches = search_words(patterns, text)

    # Print matches
    for pattern, start, end in matches:
        print(f"Word {pattern} appears from {start} to {end}")

if __name__ == "__main__":
    main()
