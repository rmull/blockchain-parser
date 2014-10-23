## Bitcoin Blockchain Parser

I am writing this to help me understand the blockchain format. I am looking
only at documentation, rather than other source codebases, because I think an
independent approach to a problem can be interesting.

# TODO:

* Rework the input to the parser so that it expects a byte stream, rather than
  a datadir. This might make it more useful if ever turned loose on live data.
* Assemble the data into a useful data structure rather than just printing the
  contents of each block.
* Create some type of interface that will allow a user to work with the
  blockchain data and get meaningful information from it.
