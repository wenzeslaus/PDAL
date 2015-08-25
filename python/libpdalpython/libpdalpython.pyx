# distutils: language = c++
from libcpp.string cimport string

cdef extern from "Pipeline.hpp" namespace "libpdalpython":
    cdef cppclass Pipeline:
        Pipeline(string) except +
        void execute() except +
        string getXML()


cdef class PyPipeline:
    cdef Pipeline *thisptr      # hold a c++ instance which we're wrapping
    def __cinit__(self, string xml):
        self.thisptr = new Pipeline(xml)
    def __dealloc__(self):
        del self.thisptr
    def get_xml(self):
        return self.thisptr.getXML()
    def execute(self):
        if not self.thisptr:
            raise Exception("not constructed!")
        self.thisptr.execute()

