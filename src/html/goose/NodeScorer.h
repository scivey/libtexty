#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>
#include "html/Node.h"

namespace texty { namespace html { namespace goose {


template<typename TStopCounter, typename TTextCollector,
  typename TTextCleaner, typename TBoostChecker>
class NodeScorer {
 protected:
  TStopCounter &stopCounter_;
  TTextCollector &textCollector_;
  TBoostChecker &boostChecker_;
  TTextCleaner textCleaner_;
  int topNodeScore_ {0};
  Node rootNode_;
  Node topNode_;
  std::map<Node, int> nodeScores_;
  std::map<Node, size_t> nodeCounts_;
  std::set<Node> parentNodes_;

  void updateNodeScore(const Node &node, int score) {
    if (nodeScores_.count(node) == 0) {
      nodeScores_.insert(std::make_pair(node, 0));
    }
    nodeScores_[node] += score;
  }

  void incrNodeCount(const Node &node) {
    if (nodeCounts_.count(node) == 0) {
      nodeCounts_.insert(std::make_pair(node, 0));
    }
    nodeCounts_[node] += 1;
  }

  void updateTextyNode(const Node &node, int boostScore) {
    if (node.hasParent()) {
      auto parent = node.parent();
      updateNodeScore(parent, boostScore);
      incrNodeCount(parent);
      parentNodes_.insert(parent);
      if (parent.hasParent()) {
        auto grandparent = parent.parent();
        updateNodeScore(grandparent, boostScore / 2);
        incrNodeCount(grandparent);
        parentNodes_.insert(grandparent);
      }
    }
  }

  bool shouldBoost(const Node &node) {
    return boostChecker_.shouldBoost(node);
  }

 public:

  NodeScorer(TStopCounter &stops, TTextCollector &collector,
      TBoostChecker &boostChecker, const Node &rootNode)
      : stopCounter_(stops),
        textCollector_(collector),
        boostChecker_(boostChecker),
        rootNode_(rootNode) {}

  int getNodeScore(const Node &node) {
    auto found = nodeScores_.find(node.getGumboNode());
    if (found == nodeScores_.end()) {
      return 0;
    }
    return found->second;
  }

  void process() {
    // Most of the numbers here have no explanation that I'm aware of.
    // They're just black magic constants copied from both the Scala
    // and Python Goose projects. (They appear to use the same constants.)

    if (topNode_.good()) {
      return;
    }

    auto nodesWithText = textCollector_.collect(rootNode_);

    double bottomNodesForNegativeScore = 0.25 * nodesWithText.size();
    double startingBoost = 1.0;
    size_t i = 0;

    for (auto node: nodesWithText) {
      double boostScore = 0;
      if (shouldBoost(node)) {
        boostScore = ((1.0 / startingBoost) * 50);
        startingBoost += 1.0;
      }
      if (nodesWithText.size() > 15) {
        if ((nodesWithText.size() - i) <= bottomNodesForNegativeScore) {
          double booster = bottomNodesForNegativeScore - ((double) nodesWithText.size() - i);
          boostScore = -1.0 * pow(booster, 2.0);
          double negScore = std::abs(boostScore);
          if (negScore > 40) {
            boostScore = 5.0;
          }
        }
      }
      auto nodeText = textCleaner_.getText(node);
      int upscore = stopCounter_.countStopwords(nodeText) + ((int) boostScore);
      updateTextyNode(node, upscore);
      i++;
    }

    int topNodeScore = 0;
    Node topNode;
    for (auto node: parentNodes_) {
      int score = nodeScores_[node.getGumboNode()];
      if (score > topNodeScore) {
        topNode = node;
        topNodeScore = score;
      } else if (!topNode.good()) {
        topNode = node;
      }
    }
    topNode_ = topNode;
    topNodeScore_ = topNodeScore;
  }

  Node getTopNode() {
    return topNode_;
  }

  int getTopNodeScore() {
    return topNodeScore_;
  }
};

}}} // texty::html::goose
