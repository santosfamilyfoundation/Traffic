import unittest

class TestNGSIM(unittest.TestCase):
    'Tutorial example for NGSIM data'

    def test_ex1(self):
        import storage
        objects = storage.loadTrajectoriesFromNgsimFile('../samples/trajectories-0400-0415.txt',100)
        for o in objects: o.plot()

class TestTrajectoryLoading(unittest.TestCase):
    'Tutorial example for NGSIM data'

    def test_ex1(self):
        import storage
        objects = storage.loadTrajectoriesFromSqlite('../samples/laurier.sqlite', 'object')

        speed = objects[0].getVelocityAtInstant(10).norm2()
        timeInterval = objects[0].getTimeInterval()
        speeds = [objects[0].getVelocityAtInstant(t).norm2() for t in range(timeInterval.first, timeInterval.last)]
        speeds = [v.norm2() for v in objects[0].getVelocities()]

        from matplotlib.pyplot import plot, close, axis
        plot(range(timeInterval.first, timeInterval.last+1), speeds)

        close('all')
        objects[0].plot()
        axis('equal')

        features = storage.loadTrajectoriesFromSqlite('../samples/laurier.sqlite', 'feature')
        objects[0].setFeatures(features)

        for f in objects[0].features:
            f.plot()
        axis('equal')


if __name__ == '__main__':
    unittest.main()
