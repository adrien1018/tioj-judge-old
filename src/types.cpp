#include "types.h"

#include <algorithm>

const int ResLimit::kNoLim;
const int Submission::kAllTestdata;

template <class T> T LimAdj::operator()(T val) const {
  if (val == ResLimit::kNoLim || factor < 0)
    return ResLimit::kNoLim;
  T result = static_cast<T>(factor * val) + constant;
  return result < 0 ? 0 : result;
}

JobGraph::VertexPtr JobGraph::InsertVertex(const Job& a) {
  vertices_.emplace_back(a);
  return vertices_.begin();
}

void JobGraph::AddEdge(JobGraph::VertexPtr from, JobGraph::VertexPtr to) {
  from->out_edges_.push_back(to);
  to->in_edges_.push_back(from);
}

std::vector<JobGraph::VertexPtr> JobGraph::RemoveVertex(
    JobGraph::VertexPtr ptr) {
  typedef JobGraph::VertexPtr VertexPtr;
  for (VertexPtr it : ptr->in_edges_) {
    it->out_edges_.erase(std::find(it->out_edges_.begin(),
                                   it->out_edges_.end(), ptr));
  }
  std::vector<VertexPtr> ret;
  for (VertexPtr it : ptr->out_edges_) {
    it->in_edges_.erase(std::find(it->in_edges_.begin(),
                                  it->in_edges_.end(), ptr));\
    if (it->in_edges_.empty()) ret.push_back(it);
  }
  vertices_.erase(ptr);
  return ret;
}

const Job& JobGraph::GetJob(JobGraph::VertexPtr ptr) {
  return ptr->job_;
}

size_t JobGraph::size() const {
  return vertices_.size();
}

bool QueueingJob::operator<(const QueueingJob& a) const {
  return JobGraph::GetJob(job).priority > JobGraph::GetJob(a.job).priority;
}

