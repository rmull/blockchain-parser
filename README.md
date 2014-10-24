# Bitcoin Blockchain Parser

I am writing this to help me understand the blockchain format. I am looking
only at documentation, rather than other source codebases, because I think an
independent approach to a problem can be interesting.

## TODO:

* Currently, the parser will break if there is a truncated blockchain
  encountered. The subparsing routines need to be able to retain their state
  and return to the caller while waiting for more data to be delivered.
* Assemble the data into a useful data structure rather than just printing the
  contents of each block.
* Create some type of interface that will allow a user to work with the
  blockchain data and get meaningful information from it.
