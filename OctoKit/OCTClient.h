//
//  OCTClient.h
//  OctoKit
//
//  Created by Josh Abernathy on 3/6/12.
//  Copyright (c) 2012 GitHub. All rights reserved.
//

#import "AFNetworking.h"
#import <ReactiveCocoa/ReactiveCocoa.h>

@class OCTUser;
@class OCTOrg;
@class OCTTeam;

// The domain for all errors originating in OCTClient.
extern NSString * const OCTClientErrorDomain;

// A request was made to an endpoint that requires authentication, and the user
// is not logged in.
extern const NSInteger OCTClientErrorAuthenticationFailed;

// The request was invalid (HTTP error 400).
extern const NSInteger OCTClientErrorBadRequest;

// The server refused to process the request (HTTP error 422).
extern const NSInteger OCTClientErrorServiceRequestFailed;

// There was a problem connecting to the server.
extern const NSInteger OCTClientErrorConnectionFailed;

// JSON parsing failed, or a model object could not be created from the parsed
// JSON.
extern const NSInteger OCTClientErrorJSONParsingFailed;

// A user info key associated with the NSURL of the request that failed.
extern NSString * const OCTClientErrorRequestURLKey;

// A user info key associated with an NSNumber, indicating the HTTP status code
// that was returned with the error.
extern NSString * const OCTClientErrorHTTPStatusCodeKey;

// Represents a single GitHub session.
@interface OCTClient : AFHTTPClient

@property (nonatomic, readonly, strong) OCTUser *user;

+ (OCTClient *)clientForUser:(OCTUser *)user;

// Enqueues a request that always fetches the latest data from the server.
//
// method      - The HTTP method to use in the request (e.g., "GET" or "POST").
// path        - The path to request, relative to the base API endpoint. This
//               path should _not_ begin with a forward slash.
// parameters  - HTTP parameters to encode and send with the request.
// resultClass - A subclass of OCTObject that the response data should be
//               returned as. If this is nil, the response is returned as the
//               parsed JSON type (a dictionary or array).
//
// Returns a signal which, upon success, will send an instance of
// `resultClass` (or an array thereof, for multi-page responses), then send
// completed.
- (RACSignal *)enqueueRequestWithMethod:(NSString *)method path:(NSString *)path parameters:(NSDictionary *)parameters resultClass:(Class)resultClass;

// Enqueues a request which will conditionally fetch the latest data from the
// server. If the latest data matches `etag`, nothing is downloaded and the call
// does not count toward the API rate limit.
//
// method          - The HTTP method to use in the request (e.g., "GET" or
//                   "POST").
// path            - The path to request, relative to the base API endpoint.
//                   This path should _not_ begin with a forward slash.
// parameters      - HTTP parameters to encode and send with the request.
// notMatchingEtag - An ETag to compare the server data against, previously
//                   retrieved from an instance of OCTResponse. If the content
//                   has not changed since, no new data will be fetched. This
//                   argument may be nil to always fetch the latest data.
// resultClass     - A subclass of OCTObject that the response data should be
//                   returned as. If this is nil, the response is returned as
//                   the parsed JSON type (a dictionary or array).
//
// Returns a signal which, upon success, will send an instance of
// OCTResponse _if new data was retrieved_. On success, the signal
// will send completed regardless of whether there was new data.
- (RACSignal *)enqueueConditionalRequestWithMethod:(NSString *)method path:(NSString *)path parameters:(NSDictionary *)parameters notMatchingEtag:(NSString *)etag resultClass:(Class)resultClass;

@end

@interface OCTClient (User)

- (RACSignal *)login;
- (RACSignal *)fetchUserInfo;

- (RACSignal *)fetchUserRepos;
- (RACSignal *)createRepoWithName:(NSString *)name description:(NSString *)description private:(BOOL)isPrivate;

@end

@interface OCTClient (Orgs)

- (RACSignal *)fetchUserOrgs;
- (RACSignal *)fetchOrgInfo:(OCTOrg *)org;

- (RACSignal *)fetchReposForOrg:(OCTOrg *)org;
- (RACSignal *)createRepoWithName:(NSString *)name org:(OCTOrg *)org team:(OCTTeam *)team description:(NSString *)description private:(BOOL)isPrivate;

- (RACSignal *)fetchTeamsForOrg:(OCTOrg *)org;

@end

@interface OCTClient (Keys)

- (RACSignal *)fetchPublicKeys;
- (RACSignal *)postPublicKey:(NSString *)key title:(NSString *)title;

@end

@interface OCTClient (Events)

// Conditionally fetches events from the logged-in user's activity stream. If
// the latest data matches `etag`, the call does not count toward the API rate
// limit.
//
// Returns a signal which will send an array of OCTEvents if data was
// downloaded. Unrecognized events will be omitted from the result.
- (RACSignal *)fetchUserEventsNotMatchingEtag:(NSString *)etag;

@end
