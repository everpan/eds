#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory>

// EDS LiveClient header file
#include "../include/LiveClient.h"

// Some common types are defined in eds namespace
using namespace eds;
// LiveClient is in eds::live namespace
using namespace eds::live;


namespace utils
{

  void sleep(float seconds)
  {
#ifdef WIN32
    DWORD t;

    t = (DWORD)(seconds * 1000.0);
    Sleep(t);
#else
    struct timespec t;

    t.tv_sec = (time_t)floor(seconds);
    t.tv_nsec = (long)((seconds - floor(seconds)) * 1000000000.0);
    nanosleep(&t, NULL);
#endif
  }

  template <typename T>
  T random(T min, T max)
  {
    double x = static_cast<double>(rand()) / RAND_MAX;
    T range = max - min;
    return min + static_cast<T>(x * range);
  }
} // namespace utils


LiveClient* initializeClient(const char* version,
                             const char* host,
                             unsigned short port)
{
  try
  {
    printf("Creating LiveClient for EDS version %s...\n", version);
    std::auto_ptr<LiveClient> client(new LiveClient(version));

    // Initialize in bidirectional (read-write) mode
    printf("Initializing LiveClient connection to %s:%u...\n", host, port);
    client->init(AccessMode_ReadWrite,
                 "0.0.0.0",             // bind to all local interfaces
                 0,                     // bind to any local port
                 host,
                 port,
                 50);

    printf("LiveClient initialized successfully!\n");
    return client.release();
  }
  catch (const BackendNotFoundError& exc)
  {
    // Couldn't load backend. Requested version might be incorrect,
    // or 'edsapi_live_*' library might be missing.
    printf("Couldn't load backend library for EDS %s (%s)\n",
      version, exc.what());
    return 0;
  }
  catch (const LiveClientError& exc)
  {
    printf("Couldn't initialize client connection object (%s)\n",
      exc.what());
    return 0;
  }
}

bool subscribePoint(LiveClient* client, const char* pointName)
{
  try
  {
    // Get lid (local id) of point by it's IESS
    int lid = client->findByIESS(pointName);
    if (lid == -1)
    {
      printf("Couldn't find point with IESS '%s'\n", pointName);
      return false;
    }

    client->setInput(lid);
    return true;
  }
  catch (const Error& exc)
  {
    printf("Failed to subscribe point '%s' (%s)\n", pointName, exc.what());
    return false;
  }
}

bool originatePoint(LiveClient* client, const char* pointName)
{
  try
  {
    // Get lid (local id) of point by it's IESS
    int lid = client->findByIESS(pointName);
    if (lid == -1)
    {
      printf("Couldn't find point with IESS '%s'\n", pointName);
      return false;
    }

    client->setOutput(lid);
    return true;
  }
  catch (const Error& exc)
  {
    printf("Failed to originate point '%s' (%s)\n", pointName, exc.what());
    return false;
  }
}

void downloadPointValues(LiveClient* client)
{
  try
  {
    do
    {
      client->synchronizeInput();
    } while (client->isUpdateRequired());
  }
  catch (const Error& exc)
  {
    printf("Failed to synchronize input point values (%s)\n", exc.what());
  }
}

void printPointValues(LiveClient* client, const char* pointName)
{
  try
  {
    int lid = client->findByIESS(pointName);
    if (lid == -1)
    {
      printf("Couldn't find point with IESS '%s'\n", pointName);
      return;
    }

    // Read value of analog point
    char quality;
    float value = client->readAnalog(lid, &quality);
    printf("lid=%d value=%f%c\n", lid, value, quality);

    // Read string field by name
    std::string iess = client->readFieldString(lid, "IESS");
    printf("lid=%d IESS=%s\n", lid, iess.c_str());

    // Read string field by id
    int unFieldId = client->fieldIdFromName("UN");
    std::string un = client->readFieldString(lid, unFieldId);
    printf("lid=%d UN=%s\n", lid, un.c_str());

    std::string t = client->readFieldString(lid, "T");
    printf("lid=%d T=%s\n", lid, t.c_str());

    // Read double field
    double tb = client->readFieldDouble(lid, "TB");
    printf("lid=%d TB=%f\n", lid, tb);
  }
  catch (const Error& exc)
  {
    printf("Failed to read values of point '%s' (%s)\n",
      pointName, exc.what());
  }
}

void uploadPointValues(LiveClient* client)
{
  try
  {
    client->synchronizeOutput();
  }
  catch (const Error& exc)
  {
    printf("Failed to synchronize output point values (%s)\n", exc.what());
  }
}

void writePoint(LiveClient* client, const char* pointName, float value)
{
  try
  {
    int lid = client->findByIESS(pointName);
    if (lid == -1)
    {
      printf("Couldn't find point with IESS '%s'\n", pointName);
      return;
    }

    client->write(lid, value, eds::Quality_Good);
  }
  catch (const Error& exc)
  {
    printf("Failed to write values of point '%s' (%s)\n",
      pointName, exc.what());
  }
}

int main(int argc, char** argv)
{
  if (argc != 4)
  {
    printf("Usage: %s version rhost rport\n", argv[0]);
    return 1;
  }

  const char* version = argv[1];
  const char* rhost = argv[2];
  unsigned short rport = (unsigned short)atoi(argv[3]);

  std::auto_ptr<LiveClient> client(initializeClient(version, rhost, rport));
  if (client.get() == 0)
    return 2;

  // Points have to be subscribed to receive value updates from server
  if (!subscribePoint(client.get(), "A1") ||
      !subscribePoint(client.get(), "A2"))
    return 3;

  // Points have to be originated to send value changes to server
  if (!originatePoint(client.get(), "AOut"))
    return 4;

  // Read point values for 60 seconds
  for (int t = 0; t < 60; ++t)
  {
    downloadPointValues(client.get());
    printf("------------------------------------------------\n");
    printPointValues(client.get(), "A1");
    printPointValues(client.get(), "A2");

    writePoint(client.get(), "AOut", utils::random<float>(0, 1));
    uploadPointValues(client.get());

    utils::sleep(1.0f);
  }

  return 0;
}
