#include <cxxtest/TestSuite.h>

#include <marsyas/system/MarSystem.h>
#include <marsyas/marsystems/Fanout.h>

using namespace std;
using namespace Marsyas;

class DummyMarSystem : public MarSystem
{
public:
  DummyMarSystem(const std::string & name):
    MarSystem("TestDummy", "name")
  {}

  virtual MarSystem* clone() const
  {
    return new DummyMarSystem(*this);
  }

  void myUpdate(MarControlPtr sender) {}

  void myProcess(realvec& in, realvec& out)
  {
    if (inObservations_ < 1 || inSamples_ < 1)
      return;

    for(mrs_natural out_observation = 0; out_observation < onObservations_; ++out_observation)
    {
      mrs_natural in_observation = out_observation % inObservations_;
      for(mrs_natural out_sample = 0; out_sample < onSamples_; ++out_sample)
      {
        mrs_natural in_sample = out_sample % inSamples_;
        out(out_observation, out_sample) = in(in_observation, in_sample);
      }
    }
  }
};


class Fanout_runner : public CxxTest::TestSuite
{
public:
  Fanout * create_network(realvec & in, realvec & out)
  {
    Fanout *fanout = new Fanout("fanout");
    fanout->setControl("mrs_natural/inSamples", 3);
    fanout->setControl("mrs_natural/inObservations", 2);
    fanout->update();

    int child_count = 3;
    for(int i = 0; i < child_count; ++i)
    {
      std::stringstream name;
      name << "child" << i;
      DummyMarSystem *child = new DummyMarSystem(name.str());
      child->setControl("mrs_natural/onSamples", 3 - i);
      child->setControl("mrs_natural/onObservations", i + 1);
      child->update();

      fanout->addMarSystem(child);
    }

    const mrs_natural in_samples = 3;
    const mrs_natural in_observations = 2;
    const mrs_natural out_samples = 3;
    const mrs_natural out_observations = 6;

    TS_ASSERT_EQUALS(fanout->getControl("mrs_natural/inSamples")->to<mrs_natural>(), in_samples);
    TS_ASSERT_EQUALS(fanout->getControl("mrs_natural/inObservations")->to<mrs_natural>(), in_observations);
    TS_ASSERT_EQUALS(fanout->getControl("mrs_natural/onSamples")->to<mrs_natural>(), out_samples);
    TS_ASSERT_EQUALS(fanout->getControl("mrs_natural/onObservations")->to<mrs_natural>(), out_observations);

    in.create(in_observations, in_samples);
    for(int o = 0; o < in_observations; ++o)
      for(int s = 0; s < in_samples; ++s)
        in(o,s) = (o+1)*10 + (s+1);

    out.create(out_observations, out_samples);

    return fanout;
  }

  void setUp()
  {
  }

  void tearDown()
  {
  }

  void test_basic_routing()
  {
    realvec in, out;
    Fanout *fanout = create_network(in, out);
    fanout->process(in, out);

    mrs_natural offset = 0;
    mrs_natural observation_counts[3] = {3,2,1};
    mrs_natural sample_counts[3] = {1,2,3};

    for(mrs_natural idx = 0; idx < 3; ++idx)
    {
      for(mrs_natural o = 0; o < observation_counts[idx]; ++o)
        for(mrs_natural s = 0; s < sample_counts[idx]; ++s)
          TS_ASSERT_EQUALS( out(o, s), in(offset + o, s) );

      offset += observation_counts[idx];
    }

    delete fanout;
  }
};

