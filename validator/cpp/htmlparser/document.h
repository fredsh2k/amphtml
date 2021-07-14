//
// Copyright 2020 The AMP HTML Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the license.
//

#ifndef HTMLPARSER__DOCUMENT_H_
#define HTMLPARSER__DOCUMENT_H_

#include <memory>
#include <vector>

#include "allocator.h"
#include "node.h"
#include "token.h"

namespace htmlparser {

class Parser;
struct ParseOptions;

// Contains pieces of information about a particular HTML parse operation.
// Clients are expected to treat all the fields as constants, but are given the
// flexibility of modifying for upstream error reporting.
struct DocumentMetadata {
 public:
  // Tells if any of the HTML, HEAD, and/or BODY elements are generated by
  // the Parser because they were missing or implicitly created before they
  // are parsed.
  // Example:
  // Original document: <html><div>foo</div></html>
  // Parsed document: <html><head></head><body><div>foo</div></body></html>
  //
  // The has_manufactured_* accounting applies only to missing
  // TokenType::START_TAG_TOKEN.
  // If any of the </html>, </head>, or </body> end tags are missing, parser
  // auto closes the elements but they are not treated as manufactured from
  // the clients perspective.
  bool has_manufactured_html = false;
  bool has_manufactured_head = false;
  bool has_manufactured_body = false;

  // HTML5 algorithm handles duplication of unique tags by merging them and
  // producing a valid HTML. However, if clients are interested in knowing if
  // the original HTML source contains duplicate elements, following bits are
  // set.
  bool duplicate_html_elements = false;
  bool duplicate_body_elements = false;
  // Set only if above duplicate bits are true.
  std::optional<LineCol> duplicate_html_element_location = std::nullopt;
  std::optional<LineCol> duplicate_body_element_location = std::nullopt;

  // If true, parsed src is missing required <!doctype html> declaration or is
  // invalid syntax or is XHTML 4 or legacy doctype.
  bool quirks_mode = false;

  // The line column position of the last element in the document. Useful for
  // error reporting at the end of the document.
  LineCol document_end_location {0, 0};

  // The actual size of html src in bytes.
  std::size_t html_src_bytes = 0;

  // The document's <base> url and target.
  // See https://developer.mozilla.org/en-US/docs/Web/HTML/Element/base
  std::pair<std::string, std::string> base_url;

  // The link rel=canonical url found in the html src. If multiple link
  // rel=canonical found the last one is recorded.
  std::string canonical_url;
};


// The document class is a wrapper for the DOM tree exposed with RootNode().
// All the nodes inside the document are owned by document. The nodes are
// destroyed when Document objects goes out of scope or deleted.
class Document {
 public:
  Document();
  ~Document() = default;


  const DocumentMetadata& Metadata() const { return metadata_; }

  // Creates a new node. The node is owned by Document and is destroyed when
  // document is destructed.
  Node* NewNode(NodeType node_type, Atom atom = Atom::UNKNOWN);

  // Returns the root node of a DOM tree. Node* owned by document.
  Node* RootNode() const { return root_node_; }

  // Returns list of nodes parsed as a document fragment. All the Nodes are
  // owned by the document.
  const std::vector<Node*> FragmentNodes() const { return fragment_nodes_; }

 private:
  // Returns a new node with the same type, data and attributes.
  // The clone has no parent, no siblings and no children.
  // The node is owned by the document and is destroyed when document is
  // destructed.
  Node* CloneNode(const Node* from);

  // The node allocator.
  std::unique_ptr<Allocator<Node>> node_allocator_;

  Node* root_node_;
  std::vector<Node*> fragment_nodes_{};
  std::size_t html_src_bytes_;
  DocumentMetadata metadata_;

  friend class Parser;
  friend std::unique_ptr<Document> Parse(std::string_view html);
  friend std::unique_ptr<Document> ParseWithOptions(
      std::string_view html, const ParseOptions& options);
  friend std::unique_ptr<Document> ParseFragment(std::string_view html,
                                                 Node* fragment_parent);
  friend std::unique_ptr<Document> ParseFragmentWithOptions(
      const std::string_view html,
      const ParseOptions& options,
      Node* fragment_parent);
};

}  // namespace htmlparser


#endif  // HTMLPARSER__DOCUMENT_H_
