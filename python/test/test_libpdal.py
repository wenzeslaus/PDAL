import libpdalpython
import unittest

xml = file('../test/data/pipeline/pipeline_read.xml','r').read()
#print xml

class TestConstruction(unittest.TestCase):

  def test_construction(self):
      r = libpdalpython.PyPipeline(xml)
      r.execute()
      self.assertEqual('foo'.upper(), 'FOO')


def test_suite():
    return unittest.TestSuite(
        [TestConstruction])

if __name__ == '__main__':
    unittest.main()
